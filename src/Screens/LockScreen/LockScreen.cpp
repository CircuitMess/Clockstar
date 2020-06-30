#include "LockScreen.h"

#include "../../CircuitWatch.h"
#include "../../Bitmaps/lock_open.hpp"
#include "../../Bitmaps/lock_closed.hpp"

#include "../../Bitmaps/analogni/analog_bg.hpp"
#include "../../Bitmaps/analogni/ticker_hour.hpp"
#include "../../Bitmaps/analogni/ticker_minute.hpp"
#include "../../Services/Kickstarter.h"

#include <Time.h>
#include <Util/Debug.h>
#include <Update/UpdateManager.h>
#include <Input/Input.h>

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>

Kickstarter* ks;

LockScreen* LockScreen::instance = nullptr;

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(1, 26);
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod>* str;

LockScreen::LockScreen(Display& display, Context* unlockedScreen) :
		Context(display), unlockedScreen(unlockedScreen),
		layers(&screen),
		bgScroll(&layers),
		bgLayoutCache(&bgScroll),
		bgLayout(&bgLayoutCache, HORIZONTAL),
		bgGrid(&bgLayout, 2),
		bgImage0(&bgLayout, 28, 64),
		bgImage1(&bgGrid, 24, 31),
		bgImage2(&bgGrid, 36, 31),
		fgLayout(&layers, VERTICAL),
		clock(&fgLayout, screen.getWidth() - 10, screen.getHeight() - 28),
		lockSlider(&fgLayout, 18, 18),
		hourTicker(&fgLayout, 34, 34), minuteTicker(&fgLayout, 44, 44){

	strip.Begin();
	strip.ClearTo(RgbColor(255, 255, 255));
	strip.SetBrightness(1);
	strip.Show();
	str = &strip;

	addSprite(&bgLayoutCache);
	addSprite(&clock);
	// addSprite(&screen);

	lockSlider.setLongListener(LockScreen::onUnlockLong);
	lockSlider.setCompleteListener(LockScreen::onUnlockComplete);
	lockSlider.setSpeed(4);

	instance = this;

	/*rtc_gpio_isolate(GPIO_NUM_16);
	rtc_gpio_isolate(GPIO_NUM_17);
	rtc_gpio_isolate(GPIO_NUM_33);
	rtc_gpio_isolate(GPIO_NUM_34);
	rtc_gpio_isolate(GPIO_NUM_36);
	rtc_gpio_isolate(GPIO_NUM_39);

	RTC.begin();*/

	/** clockYear, clockMonth, clockDay, clockHour, clockMinute, clockSecond */
	/*DateTime now = DateTime(2020, 3, 22, 15, 0, 0);
	RTC.adjust(now);*/


	buildUI();
	pack();

	Serial.println("Connecting WiFi...");
	ks = new Kickstarter();
	Serial.println("Connected!");
}

void LockScreen::btnXPress(){
	if(instance == nullptr) return;

	instance->sleepTimer = 0;

	if(instance->isSleep){
		instance->wake();
	}

	instance->lockSlider.getImageSprite()->clear(TFT_TRANSPARENT).drawIcon(lock_open, 0, 0, 18, 18, 1);
	instance->lockSlider.start();
}

void LockScreen::btnXRelease(){
	if(instance == nullptr) return;

	instance->lockSlider.stop();
	instance->lockSlider.getImageSprite()->clear(TFT_TRANSPARENT).drawIcon(lock_closed, 0, 0, 18, 18, 1);
}

void LockScreen::btnABCPress(){
	instance->lockSlider.getImageSprite()->clear(TFT_TRANSPARENT).drawIcon(lock_closed, 0, 0, 18, 18, 1);
	instance->sleepTimer = 0;
}

bool ledOn = false;
void LockScreen::start(){
	Input::getInstance()->setBtnPressCallback(BTN_N, LockScreen::btnXPress);
	Input::getInstance()->setBtnReleaseCallback(BTN_N, LockScreen::btnXRelease);


	Input::getInstance()->setBtnPressCallback(BTN_L, [](){
		ledOn = !ledOn;
		Serial.printf("Setting led to %s\n", ledOn ? "on" : "off");
		str->SetBrightness(ledOn ? 255 : 1);
		str->Show();
	});

	Input::getInstance()->setBtnPressCallback(BTN_R, [](){
		ks->update();
	});


	Input::getInstance()->setBtnPressCallback(BTN_Y, LockScreen::btnABCPress);

	UpdateManager::addListener(this);
	UpdateManager::addListener(&lockSlider);

	draw();
	screen.commit();

	sleepTimer = 0;
}

void LockScreen::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_N);
	Input::getInstance()->removeBtnReleaseCallback(BTN_N);

	Input::getInstance()->removeBtnPressCallback(BTN_L);
	Input::getInstance()->removeBtnPressCallback(BTN_R);
	Input::getInstance()->removeBtnPressCallback(BTN_Y);

	UpdateManager::removeListener(this);
	UpdateManager::removeListener(&lockSlider);
}


void LockScreen::unpack(){
	Context::unpack();

	clock.getSprite()->clear(TFT_TRANSPARENT);
	lockSlider.getImageSprite()->clear(TFT_TRANSPARENT).drawIcon(lock_closed, 0, 0, 18, 18, 1);
	bgLayoutCache.refresh();
}

void LockScreen::onUnlockLong(){
	instance->sleep();
}

void LockScreen::onUnlockComplete(){
	if(instance->unlockedScreen){
		instance->unlockedScreen->push(instance);
	}
}

void LockScreen::sleep(){
	UpdateManager::removeListener(&instance->lockSlider);
	UpdateManager::removeListener(instance);
	instance->isSleep = true;
	instance->screen.getDisplay()->setPower(false);
}

void LockScreen::wake(){
	isSleep = false;
	UpdateManager::addListener(&instance->lockSlider);
	UpdateManager::addListener(instance);
	instance->getScreen().getDisplay()->setPower(true);
}

void LockScreen::update(uint time){
	if(packed || isSleep) return;

	sleepTimer += time / 1000.0;

	if(sleepTimer > 1000 && sleepTimer < 1200){ // ehh
		lockSlider.getImageSprite()->clear(TFT_TRANSPARENT);
	}

	if(false && sleepTimer > 5000 && !isSleep){
		sleep();
	}

	draw();
	screen.commit();

	//delay(20);
}

#define RGB(R, G, B) (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3))

void LockScreen::draw(){
//	screen.getSprite()->drawIcon(analog_bg, 0, 0, 128, 128);
//
//	//hourTicker.getSprite()->drawIcon(ticker_hour, 47, 47, 34, 34);
//	//minuteTicker.getSprite()->drawIcon(ticker_minute, 42, 42, 44, 44);
//
//	hourTicker.getSprite()->clear(TFT_TRANSPARENT);
//	minuteTicker.getSprite()->clear(TFT_TRANSPARENT);
//
//	float hourAngle = 2.0f * M_PI * (float) (hour() % 12) / 12.0f;
//	float minAngle = 2.0f * M_PI * (float) minute() / 60.0f;
//
//	hourAngle = minAngle = 0;
//
//	/*for(int y = 0; y < 34; y++){
//		for(int x = 0; x < 34; x++){
//			int u = cos(-hourAngle) * x + sin(-hourAngle) * y + 17;
//			int v = -sin(-hourAngle) * x + cos(-hourAngle) * y + 17;
//			int pos = v * 34 + u;
//			if(u >= 34 || v >= 34) continue;
//			Serial.printf("u & v: %d %d\n", u, v);
//			hourTicker.getSprite()->fillRect(x, y, 1, 1, ticker_hour[pos]);
//		}
//	}
//
//	for(int y = 0; y < 44; y++){
//		for(int x = 0; x < 44; x++){
//			int u = cos(-minAngle) * x + sin(-minAngle) * y;
//			int v = -sin(-minAngle) * x + cos(-minAngle) * y;
//			int pos = v * 44 + u;
//			if(pos > (43*43)) continue;
//			minuteTicker.getSprite()->fillRect(x, y, 1, 1, ticker_minute[pos]);
//		}
//	}*/
//
//	minuteTicker.getSprite()->drawIcon(ticker_minute, 0, 0, 44, 44);
//	minuteTicker.getSprite()->setChroma(TFT_TRANSPARENT);
//	minuteTicker.getSprite()->setParent(screen.getSprite());
//	minuteTicker.getSprite()->setPos(42, 42);
//	minuteTicker.getSprite()->push();
//	//minuteTicker.getSprite()->pushRotated(minAngle * 180 / M_PI);
//
//	hourTicker.getSprite()->drawIcon(ticker_hour, 0, 0, 34, 34);
//	hourTicker.getSprite()->setChroma(TFT_TRANSPARENT);
//	hourTicker.getSprite()->setParent(screen.getSprite());
//	hourTicker.getSprite()->setPos(47, 47);
//	hourTicker.getSprite()->pushRotated(hourAngle * 180 / M_PI);
//
//	return;
	screen.clear();

	uint m = millis();

	/** BG */

	uint factor = 40000; // 5000;
	float speed = M_PI; // 13.0f / 7.0f;
	uint i = (float) abs(((int) (m * speed) % (factor * 2)) - factor) / factor * (float) bgScroll.getMaxScrollX();

	bgScroll.setScroll(i, 0);
	bgScroll.setPos(0, 0);
	bgScroll.draw();

	bgScroll.setScroll(bgScroll.getMaxScrollX() - i, 0);
	bgScroll.setPos(0, 66);
	bgScroll.draw();

#define nf 0.5
#define nfb fabs(2.0 * ((int) (bp * nf) % 255) - 255)
#define clr RGB(190, (240-bp/4), (20+bp/3))

	uint mdiffCol = 2000;
	uint mdiffPos = 500;
	uint rects = 15;
	float rectSpace = 0.5;
	float boxSize = 128.0 / rects;
	uint rectSize = rectSpace * boxSize;

	float mt = fabs(2.0f * (m % mdiffCol) / mdiffCol - 1.0);
	//float mt = cos(2.0 * M_PI * m / mdiffCol) / 2.0 + 1.0;


	for(int i = 0; i < rects+2; i++){
		for(int j = 0; j < rects+2; j++){
			float pt = (float) (i + j) / 2.0 / rects;
			//pt = sin(2 * M_PI * pt) / 2.0 + 0.5;

			uint bp = (byte) ((mt + cos(pt) / 2.0 + 0.5) / 2.0 * 254.0);
			//uint bp = (byte) ((mt + pt) / 2.0 * 254.0);

			float t = (float) (m % mdiffPos) / mdiffPos;
			int posDiff = t * boxSize - boxSize;
			int posX = i * boxSize + posDiff;
			int posY = j * boxSize + posDiff;
			//checkerSprite->fillRect(posX, posY, rectSize, rectSize, clr);
		}
	}

	Sprite* canvas = clock.getSprite();
	canvas->clear(TFT_TRANSPARENT);

	/** KIckstarter */
	canvas->setTextColor(TFT_WHITE);
	canvas->setTextFont(1);
	canvas->setTextSize(1);
	canvas->setCursor(5, 1);
	canvas->printf("Funded: %.0f\n", ks->getData().moneys);
	canvas->setCursor(5, canvas->getCursorY() + 2);
	canvas->printf("Backers: %d", ks->getData().backers);

	/** Clock */
	canvas->setTextFont(1);

	// DateTime currentTime(m / 1000); // RTC.now();
	uint _sec = second();
	uint _min = minute();
	uint _hour = hour();
	uint _day = day();
	uint _month = month();
	uint _year = year();

	// _hour / minute
	canvas->setTextSize(5);
	canvas->setTextColor(TFT_WHITE);
	canvas->setCursor(2, 23);
	if(_hour < 10) canvas->print("0");
	canvas->println(String(_hour));
	canvas->setCursor(2, canvas->getCursorY()-1);
	if(_min < 10) canvas->print("0");
	canvas->println(String(_min));

	// track
	canvas->fillRoundRect(62 + 15 - (float) abs(((m * 2 + 1) % 2000) - 1000) * 15.0f / 1000.0f, 62, 40, 6, 2, TFT_WHITE);

	// date
	canvas->setCursor(75 + (_day < 10) * 2, 72);
	//canvas->setTextColor(TFT_BLUE);
	canvas->setTextSize(1);
	canvas->print(String(_day) + "/");
	if(_month < 10) canvas->print("0");
	canvas->println(String(_month) + "");

	// year
	canvas->setCursor(77, canvas->getCursorY() + 2);
	canvas->println(String(_year));

	// seconds
	canvas->setCursor(78, 43);
	// canvas->setTextColor(TFT_RED);
	canvas->setTextSize(2);
	if(_sec < 10) canvas->print("0");
	canvas->println(String(_sec));

	lockSlider.draw();

	fgLayout.draw();
}

void LockScreen::buildUI(){
	layers.setWHType(PARENT, PARENT);
	layers.reflow();

	/** BG */

	bgScroll.setWHType(PARENT, CHILDREN);

	bgLayout.setWHType(CHILDREN, CHILDREN);
	bgLayout.setPadding(0);
	bgLayout.setGutter(2);


	bgGrid.setWHType(CHILDREN, CHILDREN);
	bgGrid.setGutter(2);

	bgGrid.addChild(&bgImage1);
	bgGrid.addChild(&bgImage2);
	bgGrid.addChild(&bgImage2);
	bgGrid.addChild(&bgImage1);
	bgGrid.reflow();
	bgGrid.setStrictPos(true);

	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.reflow();
	bgLayout.setStrictPos(true);

	bgLayoutCache.setWHType(CHILDREN, CHILDREN);
	bgLayoutCache.setWidth(0);
	bgLayoutCache.setHeight(0);

	bgLayoutCache.addChild(&bgLayout);
	bgLayoutCache.reflow();
	bgLayoutCache.repos();
	bgLayoutCache.refresh();

	bgScroll.addChild(&bgLayoutCache);
	bgScroll.reflow();
	// bgScroll.getSprite()->setParent(layers.sprite); // TODO: ??
	layers.addChild(&bgScroll);

	lockSlider.setWHType(PARENT, CHILDREN);
	lockSlider.setWidth(100);
	lockSlider.reflow();
	lockSlider.getImageSprite()->clear(TFT_TRANSPARENT).setChroma(TFT_TRANSPARENT); // .drawIcon(lock_closed, 0, 0, 18, 18, 1);

	/** FG */
	fgLayout.setWHType(PARENT, PARENT);
	fgLayout.setPadding(5);
	fgLayout.addChild(&clock);
	fgLayout.addChild(&lockSlider);
	fgLayout.reflow();
	lockSlider.reflow();

	clock.getSprite()->clear(TFT_TRANSPARENT).setChroma(TFT_TRANSPARENT);

	layers.addChild(&fgLayout);

	screen.addChild(&layers);
	screen.repos();
}
