#include "MPULocator.h"

MPU* MPULocator::mpu = nullptr;

MPU* MPULocator::getMpu(){
	return mpu;
}

void MPULocator::setMpu(MPU* mpu){
	MPULocator::mpu = mpu;
}
