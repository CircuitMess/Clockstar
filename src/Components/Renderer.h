#ifndef CIRCUITWATCH_RENDERER_H
#define CIRCUITWATCH_RENDERER_H


#include <Display/Sprite.h>
#include <Util/Task.h>
#include <UI/Screen.h>
#include <Support/Context.h>
#include <Sync/Mutex.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_CXX14
#undef radians
#undef degrees
#include <glm.hpp>
#include <ext.hpp>
#include <trigonometric.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <Motion/vec.hpp>
#include <array>

typedef vec3f vtx;
typedef glm::vec<3, float> gvtx3;
typedef glm::vec<4, float> gvtx4;
typedef glm::vec<3, uint16_t> face;

class Renderer {
public:
	Renderer(Display& display, Mutex* wireMut);

	static void taskCallback(Task* task);

	void start();
	void stop();

	void setRot(const quatf& rot, const vec3f& euler);

	void setBackground(Color background);
	void setPoint(uint point);
	void incPoint();

	static Renderer* getInstance();

	Mutex* getWireMut() const;

private:
	Task renderTask;
	Display* display;
	Sprite* canvas;

	Color background = TFT_BLACK;
	uint point = -1;

	Mutex* wireMut = nullptr;
	Mutex mut;
	quatf rot;
	vec3f euler;

	static Renderer* instance;

	std::array<gvtx4*, 8> vtxBuffer;

	// view & projection transform matrix
	glm::mat<4, 4, float> vpMat;

	void render(float dt);
	void setVPmat();
};


#endif //CIRCUITWATCH_RENDERER_H
