#include "GProg.h"
#include "../../CircuitWatch.h"

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
	Input::getInstance()->setBtnPressCallback(BTN_A, GProg::btnUp);
	Input::getInstance()->setBtnPressCallback(BTN_B, GProg::btnDown);
	Input::getInstance()->setBtnPressCallback(BTN_C, GProg::btnYes);
	Input::getInstance()->setBtnPressCallback(BTN_D, GProg::btnNo);
}

void GProg::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_A);
	Input::getInstance()->removeBtnPressCallback(BTN_B);
	Input::getInstance()->removeBtnPressCallback(BTN_C);
	Input::getInstance()->removeBtnPressCallback(BTN_D);
}
