#ifndef SMARTWATCH_MENUSCREEN_H
#define SMARTWATCH_MENUSCREEN_H

#include <Support/Context.h>
#include <UI/LinearLayout.h>
#include <UI/Image.h>
#include <UI/ScrollLayout.h>
#include <UI/GridLayout.h>
#include <Elements/GridMenu.h>

#include <string>

#define ELEMENTS 7

struct LauncherItem {
	std::string title;
	Context* context;
	Image* image;
};

class MenuScreen : public Context {
public:
	MenuScreen(Display& display);

	void start() override;
	void stop() override;
	void unpack() override;
	void draw() override;

	static void btnRPress();
	static void btnLPress();
	static void btnYPress();
	static void btnXPress();

private:
	static MenuScreen* instance;

	LinearLayout screenLayout;
	LinearLayout btnLayout;
	Image imageR;
	Image imageL;
	Image imageY;
	Image imageN;
	GridMenu menu;

	const Color colors[ELEMENTS] = { TFT_GREEN, TFT_PURPLE, TFT_WHITE, TFT_YELLOW, TFT_BLUE, TFT_LIGHTGREY, TFT_OLIVE };

	std::vector<LauncherItem> menuItems;
	void fillMenu();

	void buildUI();

};


#endif //SMARTWATCH_MENUSCREEN_H
