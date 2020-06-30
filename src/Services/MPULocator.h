#ifndef CIRCUITWATCH_MPULOCATOR_H
#define CIRCUITWATCH_MPULOCATOR_H


#include <Motion/MPU.h>

class MPULocator {
public:
	static MPU* getMpu();

	static void setMpu(MPU* mpu);

private:
	static MPU* mpu;
};


#endif //CIRCUITWATCH_MPULOCATOR_H
