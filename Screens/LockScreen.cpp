
#include <Util/Debug.h>
#include <Update/UpdateManager.h>
#include "../CircuitWatch.h"
#include "LockScreen.h"
#include "Bitmaps/Bitmaps.h"

LockScreen* LockScreen::instance = nullptr;

LockScreen::LockScreen(Display& display, Context* unlockedScreen) :
		Context(display), unlockedScreen(unlockedScreen),
		layers(&screen),
		bgScroll(&layers),
		bgLayout(&bgScroll, HORIZONTAL),
		bgGrid(&bgLayout, 2),
		bgImage0(&bgLayout, 28, 64),
		bgImage1(&bgGrid, 24, 31),
		bgImage2(&bgGrid, 36, 31),
		fgLayout(&layers, VERTICAL),
		clock(&fgLayout, screen.getWidth() - 10, screen.getHeight() - 28),
		lock(&fgLayout, 18, 18){

	instance = this;

	rtc_gpio_isolate(GPIO_NUM_16);
	rtc_gpio_isolate(GPIO_NUM_17);
	rtc_gpio_isolate(GPIO_NUM_33);
	rtc_gpio_isolate(GPIO_NUM_34);
	rtc_gpio_isolate(GPIO_NUM_36);
	rtc_gpio_isolate(GPIO_NUM_39);

	RTC.begin();

	/** clockYear, clockMonth, clockDay, clockHour, clockMinute, clockSecond */
	DateTime now = DateTime(2020, 3, 22, 15, 0, 0);
	RTC.adjust(now);

	buildUI();
	compress();
}

void LockScreen::btnXPress(){
	if(instance == nullptr) return;

	instance->lockTimer = 0;
	instance->sleepTimer = 0;

	if(instance->sleep){
		instance->sleep = false;
		UpdateManager::addListener(instance);
		instance->getScreen().getDisplay()->setPower(true);
	}

	instance->btnState = true;
	instance->lock.sprite->clear(TFT_TRANSPARENT);
	instance->lock.sprite->drawIcon(lock_open, 0, 0, 18, 18, 1);
}

void LockScreen::btnXRelease(){
	if(instance == nullptr) return;

	uint score = instance->btnStateTime;

	instance->btnState = false;
	instance->btnStateTime = 0;

	instance->lock.sprite->clear(TFT_TRANSPARENT);
	instance->lock.sprite->drawIcon(lock_closed, 0, 0, 18, 18, 1);

	// under 10 pixel move
	if((score > 40 * instance->unlockSpeed) && (score < 100 * instance->unlockSpeed)){
		// sleep


		if(!instance->sleep){
			instance->sleep = true;
			UpdateManager::removeListener(instance);
			/*instance->layers.sprite->clear(TFT_BLACK);
			instance->layers.draw();
			instance->screenDrawn = false;*/
			instance->screen.getDisplay()->setPower(false);
		}
	}
}

void LockScreen::start(){
	Input::getInstance()->setBtnPressCallback(BTN_D, btnXPress);
	Input::getInstance()->setBtnReleaseCallback(BTN_D, btnXRelease);

	UpdateManager::addListener(this);

	lockTimer = 0;
	sleepTimer = 0;
}

void LockScreen::end(){
	Input::getInstance()->removeBtnPressCallback(BTN_D);
	Input::getInstance()->removeBtnReleaseCallback(BTN_D);

	UpdateManager::removeListener(this);
}

void LockScreen::compress(){
	Context::compress();

	bgScroll.sprite->resize(1, 1);
	bgLayout.sprite->resize(1, 1);
	bgGrid.sprite->resize(1, 1);
	bgImage0.sprite->resize(1, 1);
	bgImage1.sprite->resize(1, 1);
	bgImage2.sprite->resize(1, 1);
	fgLayout.sprite->resize(1, 1);
	clock.sprite->resize(1, 1);
	lock.sprite->resize(1, 1);
	layers.sprite->resize(1, 1);

	screenDrawn = false;
}

void LockScreen::depress(){
	Context::depress();

	bgImage0.resizeSprite();
	bgImage1.resizeSprite();
	bgImage2.resizeSprite();
	clock.resizeSprite();
	lock.resizeSprite();
	layers.resizeSprite();
	fgLayout.resizeSprite();
	bgGrid.resizeSprite();
	bgLayout.resizeSprite();
	bgScroll.resizeSprite();

	bgImage0.sprite->clear(TFT_GREEN);
	bgImage1.sprite->clear(TFT_GREEN);
	bgImage2.sprite->clear(TFT_GREEN);
	clock.sprite->clear(TFT_TRANSPARENT);
	lock.sprite->clear(TFT_TRANSPARENT);
	lock.sprite->drawIcon(lock_closed, 0, 0, 18, 18, 1);
}

void LockScreen::update(uint time){
	if(!depressed || sleep) return;

	lockTimer += time;
	sleepTimer += time;

	// sleep timer
	if(false && sleepTimer > 5000 && !instance->sleep){
		instance->sleep = true;
		UpdateManager::removeListener(instance);
		instance->screen.getDisplay()->setPower(false);
	}

	if(btnState){
		btnStateTime += time;
	}

	if(btnStateTime / unlockSpeed > (fgLayout.getAvailableWidth() - lock.getWidth()) && unlockedScreen != nullptr){
		btnState = false;
		btnStateTime = 0;

		unlockedScreen->push(this);

		return;
	}

	draw();
	screen.sprite->push();
	screen.getDisplay()->commit();

	i += pow(-1, direction);
	delay(20);
	if(i > bgScroll.getMaxScrollX() || i == 0) direction = !direction;
}

#define RGB(R, G, B) (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3))

void LockScreen::draw(){
	if(!screenDrawn){
		Serial.println("Drawing screen");
		layers.draw();
		screen.sprite->push();
		screenDrawn = true;
	}

	layers.sprite->clear(TFT_BLACK);
	fgLayout.sprite->clear(TFT_TRANSPARENT);

	uint m = millis();

	/** BG */

	uint factor = 10000; // 5000;
	float speed = M_PI; // 13.0f / 7.0f;
	i = (float) abs(((int) (m * speed) % (factor * 2)) - factor) / factor * (float) bgScroll.getMaxScrollX();

	bgScroll.setScroll(i, 0);
	bgLayout.sprite->push();
	bgScroll.sprite->setPos(0, 0);
	bgScroll.sprite->push();

	bgScroll.setScroll(bgScroll.getMaxScrollX() - i, 0);
	bgLayout.sprite->push();
	bgScroll.sprite->setPos(0, 66);
	bgScroll.sprite->push();


#define clr RGB(200 + bp/5, 220 - bp/5, bp - bp/4)


	/** Checkers */
	Sprite* checkerSprite = layers.sprite;

	bool direction = 0; // ((int) (m * speed) % (factor * 2)) > factor;
	uint startX = 0, startY = 0; // 0, 22
	uint width = 4, height = 4; // 4, 4
	uint dX = 9, dY = 9; // 5, 9
	uint cols = 16, rows = 16; // 12, 9
	uint offX = (int) (m / 100 * pow(-1, direction)) % (dX * cols), offY = (int)(m / 100) % (dY * rows);
	float fraction = 1; // 1
	uint uk = cols * rows;
	uint n = (m / 30) % (int) (uk + uk * fraction);
	for(int i = _min(uk - 1, n); i >= 0 && i + uk * fraction > n; i--){
		uint j = i / cols;
		float p = (float) abs( (((int) (m / 2)) % 1000) - 500.0f) / 500.0f + (float) i / uk; p /= 2; // 0 - 1
		byte bp = (byte) (p * 254.0f); // 0 - 255
		checkerSprite->fillRect( (offX + startX + dX * (i - j * cols)) % (dX * cols), (offY + startY + j * dY) % (dY * rows), width, height, clr);
	}

	int nstart = (n > (uk * fraction)) ? n - uk : n + uk;
	int nend = (n > (uk * fraction)) ? n - uk - uk * fraction : n + uk*(1 - fraction);
	for(int i = _min(nstart, uk-1); i >= _max(0, nend); i--){
		uint j = i / cols;
		float p = (float) abs( (((int) (m / 2)) % 1000) - 500.0f) / 500.0f + (float) i / uk; p /= 2; // 0 - 1
		byte bp = (byte) (p * 254.0f); // 0 - 255
		checkerSprite->fillRect( (offX + startX + dX * (i - j * cols)) % (dX * cols), (offY + startY + j * dY) % (dY * rows), width, height, clr);
	}

	/** Clock */
	clock.sprite->clear(TFT_TRANSPARENT);
	clock.sprite->setTextFont(1);

	DateTime currentTime(m / 1000); // RTC.now();
	uint sec = currentTime.second() % 60;
	uint min = currentTime.minute();
	uint hour = currentTime.hour();
	uint day = currentTime.day();
	uint month = currentTime.month();
	uint year = currentTime.year();

	// hour / minute
	clock.sprite->setTextSize(5);
	clock.sprite->setTextColor(TFT_PURPLE);
	clock.sprite->setCursor(2, 23);
	if(hour < 10) clock.sprite->print("0");
	clock.sprite->println(String(hour));
	clock.sprite->setCursor(2, clock.sprite->getCursorY()-1);
	if(min < 10) clock.sprite->print("0");
	clock.sprite->println(String(min));

	// track
	clock.sprite->fillRoundRect(62 + 15 - (float) abs(((m * 2 + 1) % 2000) - 1000) * 15.0f / 1000.0f, 62, 40, 6, 2, TFT_PURPLE);

	// date
	clock.sprite->setCursor(75 + (day < 10) * 2, 72);
	clock.sprite->setTextColor(TFT_BLUE);
	clock.sprite->setTextSize(1);
	clock.sprite->print(String(day) + "/");
	if(month < 10) clock.sprite->print("0");
	clock.sprite->println(String(month) + "");

	// year
	clock.sprite->setCursor(77, clock.sprite->getCursorY() + 2);
	clock.sprite->println(String(year));

	// seconds
	clock.sprite->setCursor(78, 43);
	clock.sprite->setTextColor(TFT_RED);
	clock.sprite->setTextSize(2);
	if(sec < 10) clock.sprite->print("0");
	clock.sprite->println(String(sec));

	clock.sprite->push();

	if(lockTimer < 2000){
		uint newPos = lock.sprite->getPosX();
		if(fgLayout.getAvailableWidth() >= lock.getWidth() + btnStateTime / unlockSpeed){
			newPos = fgLayout.getAvailableWidth() - lock.getWidth() + fgLayout.getPadding() - btnStateTime / unlockSpeed;
		}
		lock.sprite->setPos(newPos, lock.sprite->getPosY());

		lock.sprite->push();
	}

	fgLayout.sprite->push();
	layers.sprite->push();
}

void LockScreen::buildUI(){
	layers.reflow();

	/** BG */
	bgImage0.sprite->clear(TFT_GREEN);
	bgImage1.sprite->clear(TFT_GREEN);
	bgImage2.sprite->clear(TFT_GREEN);

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

	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	/*bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);
	bgLayout.addChild(&bgGrid);
	bgLayout.addChild(&bgImage0);*/
	bgLayout.reflow();

	bgScroll.addChild(&bgLayout);
	bgScroll.reflow();
	bgScroll.sprite->setParent(layers.sprite); // TODO: ??
	layers.addChild(&bgScroll);

	/** FG */
	fgLayout.sprite->clear(TFT_TRANSPARENT);
	fgLayout.sprite->setTransparent(true);
	fgLayout.setWHType(PARENT, PARENT);
	fgLayout.setPadding(5);
	fgLayout.addChild(&clock);
	fgLayout.addChild(&lock);
	fgLayout.reflow();

	clock.sprite->clear(TFT_TRANSPARENT).setTransparent(true);
	lock.sprite->setTransparent(true).setChroma(TFT_TRANSPARENT);
	lock.sprite->clear(TFT_TRANSPARENT);
	lock.sprite->drawIcon(lock_closed, 0, 0, 18, 18, 1);

	layers.addChild(&fgLayout);

	screen.addChild(&layers);

	screenDrawn = true;
}
