#include <Update/UpdateManager.h>
#include "MotionSensor.h"

#include <FeatureExtractionModules/TimeDomainFeatures/TimeDomainFeatures.h>
#include <ClassificationModules/DTW/DTW.h>

uint kFFT_WindowSize = 256;
uint kFFT_HopSize = 128;
uint DIM = 1;
uint sample_rate = 5;

MotionSensor::MotionSensor(MPU* mpu) : mpu(mpu){

	/*pipeline.addFeatureExtractionModule(
			GRT::FFT(kFFT_WindowSize, kFFT_HopSize,
					 DIM, GRT::FFT::HAMMING_WINDOW, true, false));

	GRT::MFCC::Options options;
	options.sample_rate = sample_rate;
	options.fft_size = kFFT_WindowSize / 2;
	options.start_freq = 300;
	options.end_freq = 8000;
	options.num_tri_filter = 26;
	options.num_cepstral_coeff = 12;
	options.lifter_param = 22;
	options.use_vad = true;
	options.noise_level = 5;

	pipeline.addFeatureExtractionModule(GRT::MFCC(options));
	pipeline.setClassifier(GRT::GMM(16, true, false, 1, 100, 0.001));
	pipeline.addPostProcessingModule(GRT::ClassLabelFilter(25, 40));*/

	bool always_pick_something = false;
	double null_rej = 0.5;

	pipeline = new GRT::GestureRecognitionPipeline();
	GRT::FeatureExtraction* extractor = new GRT::TimeDomainFeatures(10, 1, 1, false, true, true, false, false);
	GRT::Classifier* classifier = new GRT::DTW(false, !always_pick_something, null_rej);

	//pipeline->addFeatureExtractionModule(*extractor);
	pipeline->setClassifier(*classifier); // use scaling, use null rejection, null rejection parameter

	//pipeline.train(training_data_manager_.getAllData())
}

void MotionSensor::train(const std::vector<Recording *> &data){
	GRT::TimeSeriesClassificationData trainingData;
	trainingData.setNumDimensions(2);

	Serial.println("Filling data");
	for(const Recording* r : data){
		GRT::MatrixFloat timeseries;
		GRT::VectorFloat sample(2);

		bool skip = false;
		for(const quatf& rot : r->track){
			if(skip){ skip = false; continue; }else{ skip = true; }
			vec3f euler = rot.euler();
			sample[0] = euler.pitch;
			sample[1] = euler.roll;
			//sample[2] = euler.roll;
			timeseries.push_back(sample);
		}

		trainingData.addSample(1, timeseries);
	}

	UBaseType_t s = uxTaskGetStackHighWaterMark(nullptr);
	UBaseType_t h = xPortGetFreeHeapSize();
	Serial.printf("free stack: %d B, free heap: %d B\n", s, h);

	Serial.println("Training");
	pipeline->train(trainingData);
	Serial.println("Trained");
}

bool MotionSensor::predict(const quatf &rot){
	vec3f euler = rot.euler();
	GRT::VectorFloat sample(2);
	sample[0] = euler.pitch;
	sample[1] = euler.roll;
	//sample[2] = euler.roll;
	pipeline->predict(sample);
	UINT predicted = pipeline->getPredictedClassLabel();
	if(predicted != 0){
		Serial.printf("Predicted %d\n", predicted);
	}
	return predicted;
}

void MotionSensor::start(){
	UpdateManager::addListener(this);
}

void MotionSensor::stop(){
	UpdateManager::removeListener(this);
}

void MotionSensor::update(uint millis){

}
