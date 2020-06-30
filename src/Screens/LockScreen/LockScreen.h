#ifndef SMARTWATCH_LOCKSCREEN_H
#define SMARTWATCH_LOCKSCREEN_H

#include <Support/Context.h>
#include <UI/LinearLayout.h>
#include <UI/Image.h>
#include <UI/ScrollLayout.h>
#include <UI/CacheLayout.h>
#include <UI/GridLayout.h>
#include <Elements/SliderElement.h>
#include <Update/UpdateListener.h>
#include <UI/Layout.h>

#include <driver/rtc_io.h>
#include <RTClib.h>
#include <UI/CustomElement.h>

class Block : public CustomElement {
public:
	using CustomElement::CustomElement;

	void draw() override{
		getSprite()->fillRect(getTotalX(), getTotalY(), getWidth(), getHeight(), C_RGB(0, 130, 255));
		Element::draw();
	}
};

class LockScreen : public Context, public UpdateListener {
public:
	LockScreen(Display& display, Context* unlockedScreen);

	void start() override;
	void stop() override;
	void unpack() override;
	void draw() override;

	static void btnXPress();
	static void btnXRelease();
	static void btnABCPress();

	static void onUnlockLong();
	static void onUnlockComplete();

	void update(uint time) override;
private:

	static LockScreen* instance;

	bool isSleep = false;
	Layout layers;
	ScrollLayout bgScroll;
	CacheLayout bgLayoutCache;
	LinearLayout bgLayout;
	GridLayout bgGrid;
	Block bgImage0;
	Block bgImage1;
	Block bgImage2;
	LinearLayout fgLayout;
	Image clock;
	SliderElement lockSlider;

	RTC_PCF8523 RTC; // PCF8563

	Context* unlockedScreen;

	uint sleepTimer = 0;

	Image minuteTicker, hourTicker;

	void sleep();
	void wake();

	void buildUI();
};


#endif //SMARTWATCH_LOCKSCREEN_H
