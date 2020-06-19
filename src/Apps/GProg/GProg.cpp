#include "GProg.h"
#include <Input/Input.h>
#include "../../CircuitWatch.h"
#include "../../Components/MotionSensor.h"

GProg* GProg::instance = nullptr;

GProg::GProg(Display& display) : Context(display), menu(&getScreen(), "GProg"){
	instance = this;

	prog = new Prog(display);

	buildUI();
	pack();
}

void GProg::draw(){
	screen.draw();
	screen.commit();
}

void GProg::btnYes(){
	if(instance == nullptr) return;

	uint i = instance->menu.getSelected();

	if(i == instance->gestures.size()){
		instance->prog->push(instance);
	}
}

void GProg::fillMenu(){

	menu.addItem("Add new");
}

void GProg::buildUI(){
	fillMenu();

	menu.setWHType(PARENT, PARENT);
	menu.setTitleColor(TFT_GREEN, TFT_BLACK);

	menu.reflow();

	screen.addChild(&menu);
	screen.repos();
}

void GProg::btnUp(){
	if(instance == nullptr) return;

	instance->screen.commit();
	instance->menu.selectPrev();
}

void GProg::btnDown(){
	if(instance == nullptr) return;

	instance->menu.selectNext();
	instance->screen.commit();
}

void GProg::btnNo(){
	if(instance == nullptr) return;

	instance->pop();
}

void GProg::start(){
	if(prog->getCache().size() > 0){
		train();
	}

	Input::getInstance()->setBtnPressCallback(BTN_L, GProg::btnUp);
	Input::getInstance()->setBtnPressCallback(BTN_R, GProg::btnDown);
	Input::getInstance()->setBtnPressCallback(BTN_Y, GProg::btnYes);
	Input::getInstance()->setBtnPressCallback(BTN_N, GProg::btnNo);
}

void GProg::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_L);
	Input::getInstance()->removeBtnPressCallback(BTN_R);
	Input::getInstance()->removeBtnPressCallback(BTN_Y);
	Input::getInstance()->removeBtnPressCallback(BTN_N);
}

uint lm = 0;
uint lastPredict = -1;

[[noreturn]] void GProg::train(){
	std::vector<Recording*>& cache = prog->getCache();
	Sprite* canvas = getScreen().getSprite();

	canvas->clear(TFT_BLACK);
	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);
	canvas->setTextPadding(20);
	canvas->println("\n\n Initializing\n");
	screen.commit();

	MotionSensor* ms = new MotionSensor(prog->getMpu());

	canvas->println(" Training");
	screen.commit();
	ms->train(cache);


	bool skip = false;
	for(;;){
		if(!prog->getMpu()->readSensor()){
			continue;
		}

		if(skip){
			skip = false;
			// continue;
		}else{
			skip = true;
		}

		canvas->setTextSize(1);
		canvas->setTextColor(TFT_WHITE);

		uint m = micros();
		uint d = m - lm;
		lm = m;

		canvas->clear(TFT_BLACK);
		canvas->setCursor(0, 10);
		canvas->printf(" Rate: %.2f Hz\n\n", 1000000.0f / d);

		if(lastPredict != -1){
			canvas->printf(" Predicted %.1f s ago\n\n", (float) (m - lastPredict) / 1000000.0f);
		}else{
			canvas->println();
		}

		vec3f euler = prog->getMpu()->getQuat().euler();
		canvas->printf(" pitch %.2f\n yaw %.2f\n roll %.2f\n", euler.pitch, euler.yaw, euler.roll);

		if(m - lastPredict < 1500000){
			canvas->fillCircle(100, 64, 10, TFT_GREEN);

			canvas->setTextSize(2);
			canvas->setTextColor(TFT_GREEN);
			canvas->setCursor(10, 100);
			canvas->print("Detected");
		}

		if(skip){
			canvas->fillCircle(120, 120, 5, TFT_RED);
		}

		screen.commit();

		if(ms->predict(prog->getMpu()->getQuat())){
			lastPredict = m;
		}
	}
}
