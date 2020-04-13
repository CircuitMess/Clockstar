#ifndef CIRCUITWATCH_PROG_H
#define CIRCUITWATCH_PROG_H


#include <Support/Context.h>
#include <Elements/GridMenu.h>
#include <Update/UpdateListener.h>
#include <Motion/MPU.h>

class Prog : public Context {
public:
	Prog(Display& display);

	void draw() override;
	void start() override;
	void stop() override;

	static void btnL();
	static void btnR();
	static void btnY();
	static void btnX();

	void unpack() override;

	static void drawLoop(Task* task);

private:
	static Prog* instance;
	GridMenu menu;
	Image addImg;

	std::vector<uint> gestures;

	bool viewing = false;
	bool drawing = false;
	bool recording = false;
	Image viewImage;
	Task drawTask;

	MPU* mpu = nullptr;
	std::pair<float, float> point;

	uint lastMicros = 0;

	void showView();
	void hideView();
	void startGestureDraw();
	void stopGestureDraw();
	void gestureDraw();

	void fillMenu();
	void buildUI();
};


#endif //CIRCUITWATCH_PROG_H
