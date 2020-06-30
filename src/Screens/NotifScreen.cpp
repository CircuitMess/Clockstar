#include <Update/UpdateManager.h>
#include <Input/Input.h>
#include "NotifScreen.h"
#include "../Services/Notification.h"
#include "../Services/NotificationService.h"
#include "../CircuitWatch.h"
#include "../Bitmaps/message.hpp"

NotifScreen* NotifScreen::instance = nullptr;

NotifScreen::NotifScreen(Display& display) : Context(display), image(&getScreen(), 1, 1){
	instance = this;
	addSprite(&image);
	pack();
}

void NotifScreen::unpack(){
	Context::unpack();
	mutex = &NotificationService::getMutex();
}

void NotifScreen::draw(){
	const auto notifications = NotificationService::getNotifications();
	const uint pad = 5;
	const uint width = getScreen().getWidth() - 2 * pad;
	const uint height = 30;
	int i = pad;

	Sprite* sprite = getScreen().getSprite();
	sprite->clear(TFT_BLACK);
	sprite->setTextSize(1);
	sprite->setTextColor(TFT_WHITE);

	for(const auto notif : notifications){
		sprite->fillRoundRect(pad, i + pad, width, height, 3, TFT_BLUE);
		sprite->drawIcon(message, pad * 2 - 2, i + pad * 2 - 3, 26, 26);
		sprite->setCursor(pad * 2 + 28, i + pad * 2 + 1); sprite->setTextFont(0);
		sprite->println(notif->title.c_str());
		sprite->setCursor(pad * 2 + 28, sprite->getCursorY() + 3); sprite->setTextFont(1);
		sprite->println(notif->body.c_str());

		i += height + pad * 2;
	}
}

void NotifScreen::start(){
	Input::getInstance()->setBtnPressCallback(BTN_N, [](){
		if(instance == nullptr) return;
		instance->pop();
	});

	UpdateManager::addListener(this);
}

void NotifScreen::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_N);

	UpdateManager::removeListener(this);
}

void NotifScreen::update(uint millis){
	draw();
	getScreen().commit();
}
