#include "Prog.h"
#include "../../CircuitWatch.h"
#include <CircuitOS.h>
#include <Devices/Motion/MPU6050_CM.h>
#include <array>
#include <Update/UpdateManager.h>
#include <Input/Input.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_CXX14
#undef radians
#undef degrees
#include <glm.hpp>
#include <trigonometric.hpp>
#include <vec3.hpp>

#include <SPI.h>

Prog* Prog::instance = nullptr;

Prog::Prog(Display& display) : Context(display), mpu(new MPU6050_CM(22, 21)),
		menu(new GridMenu(&getScreen(), 3)), addImg(menu, 30, 30){

	instance = this;
	renderer = Renderer::getInstance();
	wireMut = renderer->getWireMut();
	cache.reserve(5);

	mpu->begin();
	delay(100);
	buildUI();
	pack();
}

void Prog::drawPattern(float dt, const vec3f& velocity){
	/* // init
	point = { 0, 0 };
	mpu->resetVelocity();
	 */

	vec3f vel = velocity;

	vel.x *= 20.0f; vel.y *= 20.0f;
	if(vel.x < 0.01) vel.x = 0;
	if(vel.y < 0.01) vel.y = 0;

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
	}

	screen.getSprite()->clear(recording ? TFT_GREEN : TFT_GREENYELLOW);

	int pX = (int) point.first + 64;
	int pY = (int) point.second + 64;
	screen.getSprite()->drawRect(pX, pY, 2, 2, TFT_BLACK);

	screen.commit();
}

void Prog::showRender(){
	Sprite* canvas = getScreen().getSprite();
	canvas->setFreeFont(0);
	canvas->setTextColor(TFT_WHITE);
	canvas->setTextSize(0);

	rendering = true;
	renderer->start();
}

void Prog::hideRender(){
	renderer->stop();
	delay(50);
	rendering = false;
	draw();
}

void Prog::startReproduce(){
	Serial.println("Starting reproduction");
	instance->renderer->setBackground(TFT_BLACK);
	renderer->setPoint(0);
	instance->reproducing = 1;
	UpdateManager::addListener(this);
}

void Prog::stopReproduce(){
	UpdateManager::removeListener(this);
	instance->reproducing = 0;
}

void Prog::startRecord(){
	renderer->setBackground(TFT_GOLD);
	renderer->setPoint(0);
	recStarted = false;
	mpu->readSensor();
	startEuler = mpu->getEuler();

	gesture = new Recording();
	gesture->track.clear();

	recording = true;
	UpdateManager::addListener(this);
}

void Prog::stopRecord(){
	UpdateManager::removeListener(this);
	recording = false;
	gesture = nullptr;
}

void Prog::unpack(){
	Context::unpack();

	addImg.getSprite()->clear(TFT_ORANGE);
}

void Prog::draw(){
	Image* i = &addImg;
	i->getSprite()->setTextColor(TFT_WHITE);
	i->getSprite()->setTextFont(0); i->getSprite()->setTextSize(3); i->getSprite()->setCursor(7, 5);
	i->getSprite()->print('+');

	screen.draw();
	screen.commit();
}

void Prog::btnY(){
	if(instance == nullptr || instance->rendering) return;

	uint i = instance->menu->getSelected();

	instance->showRender();

	if(i == instance->noGestures){
		instance->startRecord();
	}else{
		instance->gesture = instance->cache[i];
		instance->startReproduce();
	}
}

void Prog::fillMenu(){
	char title[8] = { 0 };
	for(int i = 0; i < noGestures; i++){
		sprintf(title, "Rec %d", i+1);
		menu->addItem({ title, &addImg });
	}

	menu->addItem({ "New", &addImg });
}

void Prog::buildUI(){
	fillMenu();

	menu->setWHType(PARENT, PARENT);
	menu->setTitleColor(TFT_GREEN, TFT_BLACK);

	menu->reflow();

	screen.addChild(menu);
	screen.repos();
}

void Prog::btnL(){
	if(instance == nullptr || instance->rendering) return;
	instance->menu->selectPrev();
	instance->getScreen().commit();
}

void Prog::btnR(){
	if(instance == nullptr || instance->rendering) return;
	instance->menu->selectNext();
	instance->getScreen().commit();
}

void Prog::btnX(){
	if(instance == nullptr) return;
	Serial.println("X trig");

	if(instance->recording){
		delete instance->gesture;
		instance->stopRecord();
	}

	if(instance->reproducing){
		instance->stopReproduce();
	}

	if(instance->rendering){
		instance->hideRender();
	}else{
		instance->pop();
	}
}

void Prog::start(){
	Input::getInstance()->setBtnPressCallback(BTN_R, Prog::btnL);
	Input::getInstance()->setBtnPressCallback(BTN_L, Prog::btnR);
	Input::getInstance()->setBtnPressCallback(BTN_Y, Prog::btnY);
	Input::getInstance()->setBtnPressCallback(BTN_N, Prog::btnX);
}

void Prog::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_L);
	Input::getInstance()->removeBtnPressCallback(BTN_R);
	Input::getInstance()->removeBtnPressCallback(BTN_Y);
	Input::getInstance()->removeBtnPressCallback(BTN_N);

	SD.end();
}

quatf rot;
vec3f euler;
void Prog::update(uint micros){
	if(instance == nullptr || (!instance->recording && !instance->reproducing)) return;

	wireMut->lock();
	bool read = instance->mpu->readSensor();
	wireMut->unlock();

	if(!read){
		Serial.println("Sensor read err");
		return;
	}

	if(reproducing){
		if(reproducing < 100){
			rot = gesture->track[reproducing-1];
			euler = rot.euler();
			renderer->setPoint(reproducing);
			reproducing++;
		}else{
			reproducing = 1;
		}

		instance->renderer->setRot(rot, euler);
		return;
	}

	euler = mpu->getEuler();
	rot = mpu->getQuat();

	if(recording && !recStarted && (euler - startEuler).len() > 0.02f){
		recStarted = true;
		renderer->setBackground(TFT_BLUE);
	}

	if(recStarted){
		if(gesture->track.size() < 100){
			gesture->track.push_back(rot);
			renderer->incPoint();
		}else{
			hideRender();
			noGestures++;

			char* title = (char*) malloc(sizeof(char) * 10);
			sprintf(title, "Rec %d\0", noGestures);
			Image* i = new Image(menu, 30, 30);
			addSprite(i);
			i->getSprite()->clear(TFT_GOLD);
			i->getSprite()->setTextColor(TFT_BLACK);
			i->getSprite()->setTextFont(0); i->getSprite()->setTextSize(3); i->getSprite()->setCursor(5, 5);
			i->getSprite()->printf("%d", noGestures);
			menu->addItem({ "foo", i });
			menu->relocate(noGestures, noGestures-1);
			menu->setSelected(noGestures);
			menu->reflow();
			menu->repos();
			saveGesture(noGestures-1);
			stopRecord();

			Serial.printf("Free stack & heap: %dB / %dB\n", uxTaskGetStackHighWaterMark(nullptr), xPortGetFreeHeapSize());

			screen.draw();
			screen.commit();
		}
	}

	renderer->setRot(rot, euler);
}

void Prog::loadGestures(){
	patternFile.read(reinterpret_cast<uint8_t*>(&noGestures), sizeof(uint));

	for(int i = 0; i < noGestures; i++){
		char* title = (char*) malloc(sizeof(char) * 8);
		sprintf(title, "Rec %d\0", noGestures);
		Image* im = new Image(menu, 30, 30);
		addSprite(im);
		im->getSprite()->clear(TFT_GOLD);
		im->getSprite()->setTextColor(TFT_BLACK);
		im->getSprite()->setTextFont(0); im->getSprite()->setTextSize(3); im->getSprite()->setCursor(5, 5);
		im->getSprite()->printf("%d", noGestures);
		menu->addItem({ title, im });
	}

	menu->relocate(0, noGestures);
	menu->setSelected(0);
	menu->reflow();
	menu->repos();
	menu->draw();
	screen.commit();
}

void Prog::saveGesture(uint index){
	cache.push_back(gesture);
}

std::vector<Recording*>& Prog::getCache(){
	return cache;
}

MPU* Prog::getMpu() const{
	return mpu;
}