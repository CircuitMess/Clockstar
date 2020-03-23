#ifndef SMARTWATCH_LOCKSCREEN_H
#define SMARTWATCH_LOCKSCREEN_H

#include <Support/Context.h>
#include <UI/LinearLayout.h>
#include <UI/Image.h>
#include <UI/ScrollLayout.h>
#include <UI/GridLayout.h>
#include <Update/UpdateListener.h>
#include <UI/LayeredLayout.h>

#include <driver/rtc_io.h>
#include <RTClib.h>

class LockScreen : public Context, public UpdateListener {
public:
	LockScreen(Display& display, Context* unlockedScreen);

	void start() override;
	void end() override;
	void depress() override;
	void compress() override;

	void draw() override;

	static void btnXPress();
	static void btnXRelease();

	void update(uint time) override;

private:
	static LockScreen* instance;

	bool sleep = false;

	ScrollLayout bgScroll;
	LinearLayout bgLayout;
	GridLayout bgGrid;
	Image bgImage0;
	Image bgImage1;
	Image bgImage2;
	LayeredLayout layers;

	LinearLayout fgLayout;
	Image clock;
	Image lock;

	RTC_PCF8523 RTC; // PCF8563

	Context* unlockedScreen;

	const uint unlockSpeed = 5;

	unsigned i = 0;
	bool direction = false;

	bool btnState = false;
	uint btnStateTime = 0;

	uint lockTimer = 0;
	uint sleepTimer = 0;

	bool screenDrawn = false;

	void buildUI();
};


#endif //SMARTWATCH_LOCKSCREEN_H
