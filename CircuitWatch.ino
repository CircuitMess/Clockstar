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
#include <Bitmaps/Bitmaps.h>
#include <Update/UpdateManager.h>

#include "CircuitWatch.h"
#include "Screens/MenuScreen.h"
#include "Screens/LockScreen.h"

Display display(128, 128, 18, 4);
Input input;

MenuScreen* menuScreen;
LockScreen* lockScreen;

void setup(){
	Serial.begin(115200);

	menuScreen = new MenuScreen(display);
	lockScreen = new LockScreen(display, menuScreen);

	lockScreen->depress();
	lockScreen->compress();
	lockScreen->depress();
	lockScreen->start();

	input.start();
}

void loop(){
	UpdateManager::update();
}