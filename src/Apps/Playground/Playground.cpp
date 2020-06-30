#include "Playground.h"
#include "../../CircuitWatch.h"
#include "../../Components/Renderer.h"
#include "Input/Input.h"
#include "Update/UpdateManager.h"
#include "../../Services/MPULocator.h"

Playground* Playground::instance = nullptr;

Playground::Playground(Display& display) : Context(display), renderer(Renderer::getInstance()){
	instance = this;
	pack();
}

void Playground::draw(){

}

void Playground::start(){
	Input::getInstance()->setBtnPressCallback(BTN_L, [](){
		if(instance == nullptr) return;
		instance->manual = !instance->manual;
	});


	Input::getInstance()->setBtnPressCallback(BTN_N, [](){
		if(instance == nullptr) return;
		instance->pop();
	});

	UpdateManager::addListener(this);
	renderer->start();
}

void Playground::stop(){
	renderer->stop();
	delay(20);
	UpdateManager::removeListener(this);
	Input::getInstance()->removeBtnPressCallback(BTN_N);
}

void Playground::update(uint micros){
	if(manual){
		MPU* mpu = MPULocator::getMpu();
		if(mpu->readSensor()){
			renderer->setRot(mpu->getQuat(), mpu->getEuler());
		}

		return;
	}

	float d = 0.001f;
	euler.pitch += d; if(euler.pitch > M_PI) euler.pitch = -M_PI;
	euler.yaw += d; if(euler.yaw > M_PI) euler.yaw = -M_PI;
	euler.roll += d; if(euler.roll > M_PI) euler.roll = -M_PI;

	glm::quat q(glm::vec3(euler.pitch, euler.yaw, euler.roll));
	quatf rot = { q.w, q.x, q.y, q.z };

	renderer->setRot(rot, euler);
}
