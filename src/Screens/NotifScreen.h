#ifndef CIRCUITWATCH_NOTIFSCREEN_H
#define CIRCUITWATCH_NOTIFSCREEN_H


#include <Support/Context.h>
#include <Sync/Mutex.h>
#include <UI/Image.h>

class NotifScreen : public Context, public UpdateListener {
public:
	NotifScreen(Display& display);

	void draw() override;
	void start() override;
	void stop() override;

	void update(uint millis) override;

	void unpack() override;

private:
	Mutex* mutex = nullptr;
	Image image;

	static NotifScreen* instance;

};


#endif //CIRCUITWATCH_NOTIFSCREEN_H
