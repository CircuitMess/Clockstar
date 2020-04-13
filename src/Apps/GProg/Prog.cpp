#include "Prog.h"
#include "../../CircuitWatch.h"
#include <Motion/MPU9250.h>

Prog* Prog::instance = nullptr;

Prog::Prog(Display& display) : Context(display), menu(&getScreen(), 3), drawTask("DrawTask", Prog::drawLoop),
	addImg(&menu, 40, 40), viewImage(&getScreen(), display.getWidth(), display.getHeight()){

	instance = this;
	viewImage.pack();

	MPU9250* _mpu = new MPU9250();
	mpu = _mpu;
	mpu->begin();
	_mpu->calibrateAK();

	addSprite(&addImg);
	buildUI();;
	pack();
}

void Prog::gestureDraw(){
	uint m = micros();
	if(lastMicros == 0){
		lastMicros = m;
		return;
	}
	float dt = (float) (m - lastMicros) / 1000000.0f;
	lastMicros = m;

	mpu->readSensor();

	vec3f rot = mpu->getEuler();
	vec3f vel = mpu->getVelocity();

	vel.x *= 20.0f; vel.y *= 20.0f;
	if(vel.x < 0.01) vel.x = 0;
	if(vel.y < 0.01) vel.y = 0;

	Serial.println(vel.x);

	dt *= -100.0f;

	float dX = vel.x * dt;
	float dY = vel.y * dt;

	if(isinf(dX) || isinf(dY)){
		return;
	}

	point.first += dX * 1.0f;
	point.second += dY * 1.0f;

	if((fabs(point.first) + fabs(point.second) > 2) && recording == false){
		recording = true;
		viewImage.getSprite()->clear(TFT_GREENYELLOW);
	}

	int pX = (int) point.first + 64;
	int pY = (int) point.second + 64;
	viewImage.getSprite()->drawRect(pX, pY, 2, 2, TFT_BLACK);
	screen.draw();
	screen.commit();
}

void Prog::drawLoop(Task* task){
	while(instance == nullptr);

	while(instance->drawing){
		instance->gestureDraw();
		vTaskDelay(50);
	}

	instance->recording = false;
}

void Prog::showView(){
	viewImage.unpack();
	viewImage.getSprite()->clear(TFT_ORANGE);
	screen.addChild(&viewImage);

	draw();
	viewing = true;
}

void Prog::hideView(){
	screen.addChild(&menu);
	viewImage.pack();

	draw();
	viewing = false;
}

void Prog::startGestureDraw(){
	point = { 0, 0 };
	drawing = true;
	mpu->resetVelocity();
	drawTask.start();
}

void Prog::stopGestureDraw(){
	drawing = false;
}

void Prog::unpack(){
	Context::unpack();

	addImg.getSprite()->clear(TFT_ORANGE);
}

void Prog::draw(){
	screen.draw();
	screen.commit();
}

void Prog::btnY(){
	if(instance == nullptr || instance->viewing) return;

	uint i = instance->menu.getSelected();

	instance->showView();

	if(i == instance->gestures.size()){
		instance->startGestureDraw();
	}
}

void Prog::fillMenu(){
	menu.addItem({ "New", &addImg });
}

void Prog::buildUI(){
	fillMenu();

	menu.setWHType(PARENT, PARENT);
	menu.setTitleColor(TFT_GREEN, TFT_BLACK);

	menu.reflow();

	screen.addChild(&menu);
	screen.repos();
}

void Prog::btnL(){
	if(instance == nullptr || instance->viewing) return;
	instance->menu.selectPrev();
}

void Prog::btnR(){
	if(instance == nullptr || instance->viewing) return;
	instance->menu.selectNext();
}

void Prog::btnX(){
	if(instance == nullptr) return;

	if(instance->viewing){
		if(instance->drawing){
			instance->stopGestureDraw();
		}

		instance->hideView();
	}else{
		instance->pop();
	}
}

void Prog::start(){
	Input::getInstance()->setBtnPressCallback(BTN_A, Prog::btnL);
	Input::getInstance()->setBtnPressCallback(BTN_B, Prog::btnR);
	Input::getInstance()->setBtnPressCallback(BTN_C, Prog::btnY);
	Input::getInstance()->setBtnPressCallback(BTN_D, Prog::btnX);
}

void Prog::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_A);
	Input::getInstance()->removeBtnPressCallback(BTN_B);
	Input::getInstance()->removeBtnPressCallback(BTN_C);
	Input::getInstance()->removeBtnPressCallback(BTN_D);
}