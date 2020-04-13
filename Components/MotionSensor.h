#ifndef CIRCUITWATCH_MOTIONSENSOR_H
#define CIRCUITWATCH_MOTIONSENSOR_H

#include <CircuitOS.h>
#include <Motion/MPU.h>

typedef uint UINT;
typedef ulong ULONG;
#include <GRT.h>

class MotionSensor : public UpdateListener {
public:
	MotionSensor();

	void start();
	void stop();

	void update(uint millis) override;

private:

	MPU mpu;

	GRT::GestureRecognitionPipeline pipeline;

};


#endif //CIRCUITWATCH_MOTIONSENSOR_H
