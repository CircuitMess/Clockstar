#ifndef CIRCUITWATCH_PLAYGROUND_H
#define CIRCUITWATCH_PLAYGROUND_H

#include <Update/UpdateListener.h>
#include <Support/Context.h>
#include <Motion/vec.hpp>
#include <Motion/MPU.h>

class Renderer;

class Playground : public Context, public UpdateListener {
public:
	Playground(Display& display);

	void draw() override;
	void update(uint micros) override;

	void start() override;
	void stop() override;

private:
	static Playground* instance;

	bool manual = false;

	Renderer* renderer = nullptr;
	vec3f euler = { 0, 0, 0 };

};


#endif //CIRCUITWATCH_PLAYGROUND_H
