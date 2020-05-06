#include <Arduino.h>

#include <CircuitOS.h>
#include <UI/Image.h>
#include <UI/Screen.h>
#include <Util/Vector.h>
#include <UI/LinearLayout.h>
#include <UI/GridLayout.h>
#include <UI/ScrollLayout.h>
#include <Util/Task.h>
#include <Input/Input.h>
#include <Input/InputGPIO.h>
#include <Update/UpdateManager.h>
#include <Time.h>

#include "src/CircuitWatch.h"
#include "src/Screens/MenuScreen.h"
#include "src/Screens/LockScreen.h"
#include "src/Services/BluetoothService.h"
#include "src/Screens/NotifScreen.h"
#include "src/Services/Notification.h"
#include "src/Services/NotificationService.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


Display display(128, 128, 18, 3);
Input* input = nullptr;

MenuScreen* menuScreen;
LockScreen* lockScreen;
NotifScreen* notifScreen;
BluetoothService* bluetooth;

void printMemoryInfo(){

	Serial.printf("Free heap size: %d\n", xPortGetFreeHeapSize());
}

void blTask(Task* task){
	// 30kb of ram for the mere inclusion of BLE in the sketch
	bluetooth->begin();
}

void setup(){
	Serial.begin(115200);

	bluetooth = new BluetoothService(); // 90kb
	input = static_cast<Input*>(new InputGPIO());

	/** hour, minute, second, day, month, year */
	setTime(22, 03, 0, 8, 4, 2020);

	//menuScreen = new MenuScreen(display);
	//lockScreen = new LockScreen(display, menuScreen); // 0.5kb
	notifScreen = new NotifScreen(display);

	Task("BLTask", blTask, 20000).start(); // 15kb

	notifScreen->unpack(); // 80kb with smaller bg, 65kb without bg
	notifScreen->start();

	/*Notification* n = new Notification();
	n->title = "Foo title";
	n->body = "You spin me round...";
	NotificationService::addNotification(n);

	n = new Notification();
	n->title = "Bar title";
	n->body = "Lorem ipsum dolor...";
	NotificationService::addNotification(n);*/

	UpdateManager::addListener(input);
}

void loop(){
	UpdateManager::update();
	usleep(500 * 1000);
}