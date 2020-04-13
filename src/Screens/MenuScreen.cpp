#include <Bitmaps/Bitmaps.hpp>
#include "../Bitmaps/Bitmaps.hpp"
#include <Util/Debug.h>
#include "../CircuitWatch.h"
#include "MenuScreen.h"

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
	app->push(instance);
}

void MenuScreen::btnXPress(){
	if(instance == nullptr) return;

	instance->pop();
}

void MenuScreen::start(){
	Input::getInstance()->setBtnReleaseCallback(BTN_A, btnRPress);
	Input::getInstance()->setBtnPressCallback(BTN_B, btnLPress);
	Input::getInstance()->setBtnPressCallback(BTN_C, btnYPress);
	Input::getInstance()->setBtnPressCallback(BTN_D, btnXPress);

	draw();
}

void MenuScreen::stop(){
	Input::getInstance()->removeBtnReleaseCallback(BTN_A);
	Input::getInstance()->removeBtnPressCallback(BTN_B);
	Input::getInstance()->removeBtnPressCallback(BTN_C);
	Input::getInstance()->removeBtnPressCallback(BTN_D);

	menu.setSelected(0);
}

void MenuScreen::unpack(){
	Context::unpack();

	imageR.getSprite()->clear(TFT_BLACK).drawIcon(arrowRight, 0, 0, 18, 18, 1);
	imageL.getSprite()->clear(TFT_BLACK).drawIcon(arrowRight, 0, 0, 18, 18, 1);
	imageY.getSprite()->clear(TFT_BLACK).drawIcon(yes, 0, 0, 18, 18, 1);
	imageN.getSprite()->clear(TFT_BLACK).drawIcon(cross, 0, 0, 18, 18, 1);
	imageL.getSprite()->rotate(2);

	for(auto& item : menuItems){
		item.image->getSprite()->clear(TFT_WHITE);
	}
}

void MenuScreen::draw(){
	screen.draw();

	btnLayout.getSprite()->fillRect(btnLayout.getTotalX(), btnLayout.getTotalY(), btnLayout.getWidth(), 16, TFT_DARKGREEN);
	imageR.draw();

	screen.commit();
}

void MenuScreen::fillMenu(){
	for(auto& item : menuItems){
		Image* image = item.image = new Image(&menu, 35, 35);
		image->getSprite()->clear(TFT_WHITE);
		addSprite(image);
		menu.addItem({ item.title.data(), image });
	}
}

void MenuScreen::buildUI(){
	/** Buttons */
	imageR.getSprite()->setChroma(TFT_BLACK);

	imageR.getSprite()->clear(TFT_BLACK);
	imageL.getSprite()->clear(TFT_BLACK);
	imageY.getSprite()->clear(TFT_BLACK);
	imageN.getSprite()->clear(TFT_BLACK);

	imageR.getSprite()->drawIcon(arrowRight, 0, 0, 18, 18, 1);
	imageL.getSprite()->drawIcon(arrowRight, 0, 0, 18, 18, 1);
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
	menu.setTitleColor(TFT_GREEN, TFT_BLACK);

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