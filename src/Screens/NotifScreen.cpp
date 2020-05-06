#include <Update/UpdateManager.h>
#include "NotifScreen.h"
#include "../Services/Notification.h"
#include "../Services/NotificationService.h"

NotifScreen::NotifScreen(Display& display) : Context(display), image(&getScreen(), 1, 1){
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
	sprite->clear(TFT_OLIVE);
	sprite->setTextSize(1);
	sprite->setTextColor(TFT_PURPLE);

	for(const auto notif : notifications){
		sprite->fillRoundRect(pad, i + pad, width, height, 3, TFT_ORANGE);
		sprite->setCursor(pad * 2, i + pad * 2); sprite->setTextFont(0);
		sprite->println(notif->title.c_str());
		sprite->setCursor(pad * 2, sprite->getCursorY()+5); sprite->setTextFont(1);
		sprite->println(notif->body.c_str());

		i += height + pad * 2;
	}
}

void NotifScreen::start(){
	UpdateManager::addListener(this);
}

void NotifScreen::stop(){
	UpdateManager::removeListener(this);
}

void NotifScreen::update(uint millis){
	draw();
	getScreen().commit();
}
