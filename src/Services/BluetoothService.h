#ifndef CIRCUITWATCH_BLUETOOTHSERVICE_H
#define CIRCUITWATCH_BLUETOOTHSERVICE_H

#define CONFIG_BT_ENABLED
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Update/UpdateListener.h>
#include <Sync/Mutex.h>

class BluetoothService : public UpdateListener {
public:
	BluetoothService();

	void begin();

	void update(uint millis) override;

private:
	const BLEUUID ALERT_DISPLAY_SERVICE_UUID = BLEUUID("3db02924-b2a6-4d47-be1f-0f90ad62a048");
	const BLEUUID DISPLAY_DATA_CHARACTERISTIC_UUID = BLEUUID("8d8218b6-97bc-4527-a8db-13094ac06b1d");
	const BLEUUID DISPLAY_INFO_CHARACTERISTIC_UUID = BLEUUID("b7b0a14b-3e94-488f-b262-5d584a1ef9e1");

	BLEServer *server;

	BLEService* service;
	BLECharacteristic* charData;
	BLECharacteristic* charInfo;
	BLEAdvertising *advertising;

	char address[6*3] = { 0 };
	bool connected = false;

	Mutex* notifMutex = nullptr;

	void setupServer();

};


#endif //CIRCUITWATCH_BLUETOOTHSERVICE_H
