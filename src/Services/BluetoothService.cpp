#include <Update/UpdateManager.h>
#include "BluetoothService.h"
#include "Notification.h"
#include "NotificationService.h"
#include <string>
#include <Time.h>

#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <cstring>

struct InfoData {
	unsigned long long time;
	byte bat;
};

class InfoCallback : public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* pTimeCharacteristic) override {
		std::string str = pTimeCharacteristic->getValue();
		const InfoData* data = reinterpret_cast<const InfoData*>(str.c_str());
		Serial.printf("Got %d bytes data, buffer is %d bytes, sizeof byte: %d, sizeof ulong: %d\n", str.size(), sizeof(InfoData), sizeof(byte), sizeof(unsigned long long));
		Serial.printf("Bat: %d%%, time: %lld\n", data->bat, data->time);
		setTime(data->time/1000);
		//adjustTime(1000 * 60 * 60 * 2);
	}
};

class DataCallback : public BLECharacteristicCallbacks {
public:
	DataCallback(Mutex* mutex) : mutex(mutex){ }

private:
	void onWrite(BLECharacteristic* pCharacteristic) override {
		std::string str = pCharacteristic->getValue();
		BLEUUID uuid = pCharacteristic->getUUID();
		std::string uuid_s = uuid.toString();

		Serial.printf("Received data %d bytes\n", sizeof(str));

		int aLen = 11;
		byte tLen = 22;
		byte bLen = 33;
		int i = 0;
		Notification* notif = new Notification();
		//memcpy(&notif->id, &str[i], sizeof(int)); i += sizeof(int);
		//memcpy(&notif->timestamp, &str[i], sizeof(unsigned long long)); i += sizeof(unsigned long long);
		//Serial.printf("Got notif ID %d, timestamp: %llu, app: [REDACTED]\n", notif->id, notif->timestamp/*, notif->app.c_str()*/);

		//memcpy(&aLen, &str[i], sizeof(int)); i += sizeof(int);
		memcpy(&tLen, &str[i], sizeof(byte)); i += sizeof(byte);
		memcpy(&bLen, &str[i], sizeof(byte)); i += sizeof(byte);
		Serial.printf("aLen / tLen / bLen : %d / %d / %d\n", aLen, tLen, bLen);
		if(bLen < 0 || bLen > 20){
			bLen = 0;
		}

		//std::vector<char> aData(aLen+1, 0);
		std::vector<char> tData(tLen+1, 0);
		std::vector<char> bData(bLen+1, 0);
		//std::strncpy(aData.data(), &str[i], aLen); i += aLen;
		std::strncpy(tData.data(), &str[i], tLen); i += tLen;
		std::strncpy(bData.data(), &str[i], bLen); i += bLen;
		//Serial.printf("Got notif ID %d, timestamp: %llu, app: [REDACTED]\n", notif->id, notif->timestamp/*, notif->app.c_str()*/);

		//notif->app = aData.data();
		notif->title = tData.data();
		notif->body = bData.data();


		Serial.printf("Title: %s\n", notif->title.c_str());
		Serial.printf("Body : %s\n", notif->body.c_str());

		NotificationService::addNotification(notif);
	}

private:
	Mutex* mutex = nullptr;
};

class ServerCallback : public BLEServerCallbacks {
public:
	ServerCallback(bool* connected) : connected(connected){ }

	void onConnect(BLEServer* pServer) override {
		Serial.println("Phone connected");
		*connected = true;
	}

	void onDisconnect(BLEServer* pServer) override {
		Serial.println("Phone disconnected");
		*connected = false;
	}

private:
	bool* connected;

};

void BluetoothService::setupServer(){
	server = BLEDevice::createServer();
	server->setCallbacks(new ServerCallback(&connected));
	service = server->createService(ALERT_DISPLAY_SERVICE_UUID );

	/*BLECharacteristic* pCharacteristic = service->createCharacteristic(DISPLAY_DATA_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
	pCharacteristic->setWriteProperty(true);*/

	charData = new BLECharacteristic(DISPLAY_DATA_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR); //Request MTU=500 from client
	charData->setCallbacks(new DataCallback(notifMutex));
	service->addCharacteristic(charData);

	charInfo = new BLECharacteristic(DISPLAY_INFO_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
	charInfo->setCallbacks(new InfoCallback());
	service->addCharacteristic(charInfo);

	service->start();

	// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml
	advertising = server->getAdvertising();
	advertising->setAppearance(320); // 320 - Generic Display
	advertising->start();
}

BluetoothService::BluetoothService(){
	BLEDevice::init("SmartWatch");
}

void BluetoothService::begin(){
	setupServer();

	const uint8_t* addr = esp_bt_dev_get_address();
	sprintf(address, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	Serial.printf("Bluetooth initialized with address: %s\n", address);

	notifMutex = &NotificationService::getMutex();
}

void BluetoothService::update(uint millis){
	if(!connected) return;
}
