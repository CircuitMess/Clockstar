#ifndef CIRCUITWATCH_PROG_H
#define CIRCUITWATCH_PROG_H


#include <SD.h>
#include <Support/Context.h>
#include <Elements/GridMenu.h>
#include <Update/UpdateListener.h>
#include <Motion/MPU.h>
#include "../../Components/Renderer.h"
#include "Recording.h"

class Prog : public Context, public UpdateListener {
public:
	Prog(Display& display);

	void draw() override;
	void start() override;
	void stop() override;

	static void btnL();
	static void btnR();
	static void btnY();
	static void btnX();

	void update(uint millis) override;

	void unpack() override;

	std::vector<Recording*>& getCache();

	MPU* getMpu() const;

private:
	std::vector<Recording*> cache;

	static Prog* instance;
	GridMenu* menu;
	Image addImg;

	uint noGestures = 0;

	bool rendering = false;
	uint reproducing = 0;
	bool recording = false;
	bool recStarted = false;
	vec3f startEuler = { 0, 0, 0 };

	Mutex* wireMut;
	MPU* mpu = nullptr;
	std::pair<float, float> point;

	Renderer* renderer = nullptr;

	Recording* gesture;

	void showRender();
	void hideRender();
	void startReproduce();
	void stopReproduce();
	void startRecord();
	void stopRecord();
	void drawPattern(float dt, const vec3f& vel);

	File patternFile;
	void loadGestures();
	void saveGesture(uint index);

	void fillMenu();
	void buildUI();
};


#endif //CIRCUITWATCH_PROG_H
