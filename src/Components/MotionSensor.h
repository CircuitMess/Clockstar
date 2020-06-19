#ifndef CIRCUITWATCH_MOTIONSENSOR_H
#define CIRCUITWATCH_MOTIONSENSOR_H

#include <CircuitOS.h>
#include <Motion/MPU.h>

typedef uint UINT;
typedef ulong ULONG;
#include <GRT.h>
#include <CoreModules/GestureRecognitionPipeline.h>
#include "../Apps/GProg/Recording.h"

class MotionSensor : public UpdateListener {
public:
	MotionSensor(MPU* mpu);

	void start();
	void stop();

	void update(uint millis) override;

	void train(const std::vector<Recording*>& data);
	bool predict(const quatf& rot);

private:

	MPU* mpu;

	GRT::GestureRecognitionPipeline* pipeline;

};


#endif //CIRCUITWATCH_MOTIONSENSOR_H
