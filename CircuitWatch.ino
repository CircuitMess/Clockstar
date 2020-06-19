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

#include "src/CircuitWatch.h"
#include "src/Screens/MenuScreen.h"
#include "src/Screens/LockScreen/LockScreen.h"
#include "src/Apps/GProg/GProg.h"


Display display(128, 128, 19, 0);

Context* menuScreen;
Context* lockScreen;

void setup(){
	Serial.begin(115200);
	display.begin();
	display.setPower(true);
	Task::setPinned(true);

	pinMode(19, OUTPUT);
	digitalWrite(19, HIGH);

	new Renderer(display, new Mutex());

	/*Serial.printf("Free stack: %d B\n", uxTaskGetStackHighWaterMark(nullptr));
	Serial.printf("Free head: %d B\n", xPortGetFreeHeapSize());*/

	/** hour, minute, second, day, month, year */
	setTime(22, 03, 0, 8, 4, 2020);

	/** ### Input ### */
	Input* input = new InputGPIO();
	UpdateManager::addListener(input);

	/** ### Setup ### */
	lockScreen = new LockScreen(display, new MenuScreen(display));
	lockScreen->unpack();
	lockScreen->start();
	Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());

	delay(20);

	// UpdateManager task needs 40kb stack to run properly because the ML library does everything on the stack.
	// For some reason the pattern recognition stuff runs in the UpdateManager task. Needs checking and optimization
	UpdateManager::startTask();

	vTaskDelete(nullptr);
}

void loop(){
	//UpdateManager::update();
	delay(1000);
}