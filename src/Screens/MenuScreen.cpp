#include "MenuScreen.h"
#include "../CircuitWatch.h"
#include <Util/Debug.h>
#include <UI/BitmapElement.h>
#include <Input/Input.h>
#include "../Apps/Playground/Playground.h"
#include "../Apps/GProg/GProg.h"
#include "../Bitmaps/arrow_right.hpp"
#include "../Bitmaps/yes.hpp"
#include "../Bitmaps/cross.hpp"
#include "../Bitmaps/apps/apps.hpp"
#include "NotifScreen.h"

MenuScreen* MenuScreen::instance = nullptr;

MenuScreen::MenuScreen(Display& display) :
	Context(display),
	screenLayout(&screen, HORIZONTAL),
	btnLayout(&screenLayout, VERTICAL),
	imageR(&btnLayout, 18, 18),
	imageL(&btnLayout, 18, 18),
	imageY(&btnLayout, 18, 18),
	imageN(&btnLayout, 18, 18),
	menu(&screenLayout, 2){

	addSprite(&imageR);
	addSprite(&imageL);
	addSprite(&imageY);
	addSprite(&imageN);

	instance = this;

	menuItems.push_back({ "Programming", new GProg(display), new BitmapElement(&menu, app_programming, 35, 35) });
	menuItems.push_back({ "Playground", new Playground(display), new BitmapElement(&menu, app_playground, 35, 35) });
	menuItems.push_back({ "Notifications", new NotifScreen(display), new BitmapElement(&menu, app_notifications, 35, 35) });
	menuItems.push_back({ "Gesture", nullptr, new BitmapElement(&menu, app_gesture, 35, 35) });
	menuItems.push_back({ "Settings", nullptr, new BitmapElement(&menu, app_settings, 35, 35) });

	buildUI();
	pack();
}

void MenuScreen::btnRPress(){
	if(instance == nullptr) return;

	instance->menu.selectNext();
	instance->screen.commit();
}

void MenuScreen::btnLPress(){
	if(instance == nullptr) return;

	instance->menu.selectPrev();
	instance->screen.commit();
}

void MenuScreen::btnYPress(){
	if(instance == nullptr) return;

	Context* app = instance->menuItems[instance->menu.getSelected()].context;
	if(app == nullptr) return;
	app->push(instance);
}

void MenuScreen::btnXPress(){
	if(instance == nullptr) return;

	instance->pop();
}

void MenuScreen::start(){
	Input::getInstance()->setBtnPressCallback(BTN_L, btnRPress);
	Input::getInstance()->setBtnPressCallback(BTN_R, btnLPress);
	Input::getInstance()->setBtnPressCallback(BTN_Y, btnYPress);
	Input::getInstance()->setBtnPressCallback(BTN_N, btnXPress);

	draw();
}

void MenuScreen::stop(){
	Input::getInstance()->removeBtnPressCallback(BTN_L);
	Input::getInstance()->removeBtnPressCallback(BTN_R);
	Input::getInstance()->removeBtnPressCallback(BTN_Y);
	Input::getInstance()->removeBtnPressCallback(BTN_N);

	menu.setSelected(0);
}

void MenuScreen::unpack(){
	Context::unpack();

	imageR.getSprite()->clear(TFT_BLACK).drawIcon(arrow_right, 0, 0, 18, 18, 1);
	imageL.getSprite()->clear(TFT_BLACK).drawIcon(arrow_right, 0, 0, 18, 18, 1);
	imageY.getSprite()->clear(TFT_BLACK).drawIcon(yes, 0, 0, 18, 18, 1);
	imageN.getSprite()->clear(TFT_BLACK).drawIcon(cross, 0, 0, 18, 18, 1);
	imageL.getSprite()->rotate(2);

	for(auto& item : menuItems){
		item.image->getSprite()->clear(TFT_WHITE);
	}
}

void MenuScreen::draw(){
	screen.draw();

	btnLayout.getSprite()->fillRect(btnLayout.getTotalX(), btnLayout.getTotalY(), btnLayout.getWidth(), 16, TFT_DARKCYAN);
	imageR.draw();

	screen.commit();
}

void MenuScreen::fillMenu(){
	menu.clearItems();

	for(auto& item : menuItems){
		menu.addItem({ item.title.data(), item.image });
	}
}

void MenuScreen::buildUI(){
	/** Buttons */
	imageR.getSprite()->setChroma(TFT_BLACK);

	imageR.getSprite()->clear(TFT_BLACK);
	imageL.getSprite()->clear(TFT_BLACK);
	imageY.getSprite()->clear(TFT_BLACK);
	imageN.getSprite()->clear(TFT_BLACK);

	imageR.getSprite()->drawIcon(arrow_right, 0, 0, 18, 18, 1);
	imageL.getSprite()->drawIcon(arrow_right, 0, 0, 18, 18, 1);
	imageY.getSprite()->drawIcon(yes, 0, 0, 18, 18, 1);
	imageN.getSprite()->drawIcon(cross, 0, 0, 18, 18, 1);

	imageL.getSprite()->rotate(2);

	btnLayout.setWHType(FIXED, PARENT);
	btnLayout.setWidth(28);
	btnLayout.setPadding(5);
	btnLayout.setGutter(15);
	//btnLayout.setBorder(1, TFT_RED);

	btnLayout.addChild(&imageR);
	btnLayout.addChild(&imageL);
	btnLayout.addChild(&imageY);
	btnLayout.addChild(&imageN);

	/** Menu */

	menu.setWHType(FIXED, PARENT);
	menu.setWidth(100);
	menu.setTitleColor(TFT_BLUE, TFT_BLACK);

	fillMenu();

	/** Layout */

	screenLayout.setWHType(PARENT, PARENT);
	screenLayout.addChild(&menu).addChild(&btnLayout);

	screenLayout.reflow();
	menu.reflow();
	btnLayout.reflow();

	screen.addChild(&screenLayout);
	screen.repos();
}