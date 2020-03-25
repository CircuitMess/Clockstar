#ifndef SMARTWATCH_MENUSCREEN_H
#define SMARTWATCH_MENUSCREEN_H

#include <Support/Context.h>
#include <UI/LinearLayout.h>
#include <UI/Image.h>
#include <UI/ScrollLayout.h>
#include <UI/GridLayout.h>

#define ELEMENTS 7

class MenuScreen : public Context {
public:
	MenuScreen(Display& display);


	void start() override;
	void stop() override;
	void unpack() override;
	void draw() override;

	static void btnRPress();
	static void btnLPress();
	static void btnXPress();

private:
	static MenuScreen* instance;

	LinearLayout screenLayout;
	LinearLayout btnLayout;
	Image imageR;
	Image imageL;
	Image imageY;
	Image imageN;
	ScrollLayout mainScroll;
	GridLayout grid;
	Vector<Image*> gridImages;

	uint selected = 0;
	const Color colors[ELEMENTS] = { TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_YELLOW, TFT_BLUE, TFT_LIGHTGREY, TFT_OLIVE };

	bool scroll();
	void selectElement(uint element);

	void buildUI();

};


#endif //SMARTWATCH_MENUSCREEN_H
