#include <Update/UpdateManager.h>
#include "MotionSensor.h"

#include <GRT.h>
#include "MFCC.h"

uint kFFT_WindowSize = 256;
uint kFFT_HopSize = 128;
uint DIM = 1;
uint sample_rate = 5;

MotionSensor::MotionSensor() : mpu(){

	pipeline.addFeatureExtractionModule(
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
	pipeline.addPostProcessingModule(GRT::ClassLabelFilter(25, 40));
}

void MotionSensor::start(){
	UpdateManager::addListener(this);
}

void MotionSensor::stop(){
	UpdateManager::removeListener(this);
}

void MotionSensor::update(uint millis){

}
