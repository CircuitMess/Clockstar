#include <Arduino.h>

#include <CircuitOS.h>
#include <UI/Image.h>
#include <UI/Screen.h>
#include <Util/Vector.h>
#include <UI/LinearLayout.h>
#include <UI/GridLayout.h>
#include <UI/ScrollLayout.h>
#include <Util/Task.h>
#include <Input/InputGPIO.h>
#include <Update/UpdateManager.h>
#include <Time.h>
#include <Devices/Motion/MPU6050_CM.h>

#include "src/CircuitWatch.h"
#include "src/Screens/MenuScreen.h"
#include "src/Screens/LockScreen/LockScreen.h"
#include "src/Apps/GProg/GProg.h"
#include "src/Services/BluetoothService.h"
#include "src/Screens/NotifScreen.h"
#include "src/Services/MPULocator.h"
#include "src/Services/NotificationService.h"
#include "src/Services/Notification.h"


Display display(128, 128, 19, 1);

Context* menuScreen;
Context* lockScreen;

BluetoothService* bluetooth;

void setup(){
	Serial.begin(115200);

	pinMode(26, OUTPUT);
	digitalWrite(26, LOW);

	psramInit();
	Serial.print("PSRAM found: ");
	Serial.println(psramFound());

	display.begin();
	display.setPower(true);
	Task::setPinned(true);

	pinMode(19, OUTPUT);
	digitalWrite(19, HIGH);

	new Renderer(display, new Mutex());

	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());
	//bluetooth = static_cast<BluetoothService*>(ps_malloc(sizeof(BluetoothService)));
	Serial.printf("Location of bluetooth: %x\n", bluetooth);
	//bluetooth = new (bluetooth) BluetoothService(); // 90kb
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());
	//bluetooth->begin();
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());

	/*Serial.printf("Free stack: %d B\n", uxTaskGetStackHighWaterMark(nullptr));
	Serial.printf("Free head: %d B\n", xPortGetFreeHeapSize());*/

	/** hour, minute, second, day, month, year */
	setTime(22, 03, 0, 8, 4, 2020);

	/** ### Input ### */
	Input* input = new InputGPIO();
	UpdateManager::addListener(input);

	MPULocator::setMpu(new MPU6050_CM(22, 21));

	/** ### Setup ### */
	lockScreen = new LockScreen(display, new MenuScreen(display));
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());
	lockScreen->unpack();
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());
	lockScreen->start();
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());

	delay(20);

	// UpdateManager task needs 40kb stack to run properly because the ML library does everything on the stack.
	// For some reason the pattern recognition stuff runs in the UpdateManager task. Needs checking and optimization
	UpdateManager::setStackSize(10000);
	UpdateManager::startTask(8 | portPRIVILEGE_BIT);
	

	disableCore0WDT();
	disableCore1WDT();

	vTaskDelete(nullptr);
}

void loop(){
	//UpdateManager::update();
	delay(1000);
}