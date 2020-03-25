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
	mainScroll(&screenLayout),
	grid(&mainScroll, 2){

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

	instance->selectElement((instance->selected + 1) % ELEMENTS);
}

void MenuScreen::btnLPress(){
	if(instance == nullptr) return;

	if(instance->selected == 0){
		instance->selectElement(ELEMENTS - 1);
	}else{
		instance->selectElement(instance->selected - 1);
	}
}

void MenuScreen::btnXPress(){
	if(instance == nullptr) return;

	instance->pop();
}

void MenuScreen::start(){
	Input::getInstance()->setBtnReleaseCallback(BTN_A, btnRPress);
	Input::getInstance()->setBtnPressCallback(BTN_B, btnLPress);
	Input::getInstance()->setBtnPressCallback(BTN_D, btnXPress);

	draw();
}

void MenuScreen::stop(){
	Input::getInstance()->removeBtnReleaseCallback(BTN_A);
	Input::getInstance()->removeBtnPressCallback(BTN_B);
	Input::getInstance()->removeBtnPressCallback(BTN_D);

	gridImages[selected]->setBorderWidth(0);
	gridImages[0]->setBorderWidth(3);
	selected = 0;
	scroll();
}

void MenuScreen::unpack(){
	Context::unpack();

	for(int i = 0; i < ELEMENTS; i++){
		gridImages[i]->getSprite()->clear(colors[i]);
	}

	imageR.getSprite()->clear(TFT_BLACK).drawIcon(arrowRight, 0, 0, 18, 18, 1);
	imageL.getSprite()->clear(TFT_BLACK).drawIcon(arrowRight, 0, 0, 18, 18, 1);
	imageY.getSprite()->clear(TFT_BLACK).drawIcon(yes, 0, 0, 18, 18, 1);
	imageN.getSprite()->clear(TFT_BLACK).drawIcon(cross, 0, 0, 18, 18, 1);
	imageL.getSprite()->rotate(2);
}

void MenuScreen::draw(){
	screen.draw();
	screen.commit();
}

void MenuScreen::buildUI(){
	/** Buttons */
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

	/** Grid */

	for(int i = 0; i < ELEMENTS; i++){
		Image* gridImage = new Image(&grid, 35, 35);
		gridImages.push_back(gridImage);

		gridImage->getSprite()->clear(colors[i]);
		gridImage->setBorderColor(TFT_RED);
		grid.addChild(gridImage);

		addSprite(gridImage);
	}

	gridImages.front()->setBorderWidth(3);

	grid.setWHType(PARENT, CHILDREN);
	grid.setPadding(10).setGutter(10);

	/** Layout */

	mainScroll.setWHType(FIXED, PARENT);
	mainScroll.setWidth(100);
	//mainScroll.setBorder(1, TFT_RED);
	mainScroll.addChild(&grid);

	screenLayout.setWHType(PARENT, PARENT);
	screenLayout.addChild(&mainScroll).addChild(&btnLayout);

	screenLayout.reflow();
	mainScroll.reflow();
	grid.reflow();
	btnLayout.reflow();

	screen.addChild(&screenLayout);
	screen.repos();
}

bool MenuScreen::scroll(){
	Image* gridImage = gridImages[selected];

	uint elStart = grid.getPadding() + (selected/2) * (gridImage->getHeight() + grid.getGutter());
	uint elEnd = elStart + gridImage->getHeight();

	uint screenStart = mainScroll.getScrollY();
	uint screenEnd = screenStart + screen.getHeight();

	uint newScroll = mainScroll.getScrollY();
	Serial.println("Element end / screen end: " + String(elEnd) + " / " + String(screenEnd));
	if(elStart < screenStart){
		newScroll = elStart;

		if(newScroll == grid.getPadding()){
			newScroll = 0;
		}else{
			newScroll -= grid.getGutter() / 2;
		}
	}else if(elEnd > screenEnd){
		newScroll += elEnd - screenEnd;

		Serial.println("A / B: " + String(newScroll + screen.getHeight()) + " / " + String(grid.getHeight() - grid.getPadding()));
		if(newScroll + screen.getHeight() == grid.getHeight() - grid.getPadding()){
			newScroll += grid.getPadding();
		}else{
			newScroll += grid.getGutter() / 2;
		}
	}

	if(newScroll == mainScroll.getScrollY()){
		return false;
	}

	mainScroll.setScroll(0, newScroll);
	return true;
}

void MenuScreen::selectElement(uint element){
	gridImages[selected]->setBorderWidth(0);
	gridImages[selected]->draw();
	gridImages[element]->setBorderWidth(3);
	gridImages[element]->draw();

	selected = element;

	if(scroll()){
		mainScroll.clear();
		mainScroll.draw();
	}

	screen.commit();
}
