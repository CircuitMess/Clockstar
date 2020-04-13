#ifndef CIRCUITWATCH_GPROG_H
#define CIRCUITWATCH_GPROG_H


#include <Elements/ListMenu.h>
#include <Support/Context.h>
#include "Prog.h"

class GProg : public Context {
public:
	GProg(Display& display);

	void draw() override;
	void start() override;
	void stop() override;

	static void btnUp();
	static void btnDown();
	static void btnYes();
	static void btnNo();

private:
	static GProg* instance;
	Prog* prog = nullptr;
	ListMenu menu;

	std::vector<uint> gestures;

	void fillMenu();
	void buildUI();
};


#endif //CIRCUITWATCH_GPROG_H
