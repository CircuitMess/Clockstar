#include "Renderer.h"

Renderer* Renderer::instance = nullptr;


const std::array<const vtx, 8> vertices PROGMEM = {
		vtx(-0.5, -0.5, -0.5), vtx(-0.5, -0.5, 0.5),
		vtx(-0.5, 0.5, -0.5), vtx(-0.5, 0.5, 0.5),
		vtx(0.5, -0.5, -0.5), vtx(0.5, -0.5, 0.5),
		vtx(0.5, 0.5, -0.5), vtx(0.5, 0.5, 0.5),
};

const std::array<const face, 12> faces PROGMEM = {
		face(0, 6, 4), face(0, 2, 6),
		face(0, 3, 2), face(0, 1, 3),
		face(2, 7, 6), face(2, 3, 7),
		face(4, 6, 7), face(4, 7, 5),
		face(0, 4, 5), face(0, 5, 1),
		face(1, 5, 7), face(1, 7, 3),
};

const std::array<const Color, 12> colors PROGMEM = {
		TFT_RED, TFT_GREEN, TFT_BLUE, TFT_CYAN, TFT_MAGENTA, TFT_YELLOW,
		TFT_OLIVE, TFT_PURPLE, TFT_WHITE, TFT_VIOLET, TFT_LIGHTGREY, TFT_GOLD
};

Renderer::Renderer(Display& display, Mutex* wireMut) : display(&display), canvas(display.getBaseSprite()), vpMat(1),
								   renderTask("RenderTask", Renderer::taskCallback, 4096, this), wireMut(wireMut){
	instance = this;

	for(int i = 0; i < vtxBuffer.size(); i++){
		vtxBuffer[i] = new gvtx4(0);
	}

	setVPmat();
}

void Renderer::setVPmat(){
	glm::mat<4, 4, float> view = glm::lookAt(gvtx3(0, -0.1, 2), gvtx3(0, 0, 0), gvtx3(0, 0, 1));
	glm::mat<4, 4, float> projection = glm::perspective((float) M_PI_2, 1.0f, 0.1f, 5.0f);
	vpMat = projection * view;
}

void Renderer::render(float dt){
	canvas->clear(background);
	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setTextColor(TFT_WHITE);

	mut.lock();
	volatile quatf _rot = rot;
	volatile vec3f _euler = euler;
	mut.unlock();

	glm::quat q = { _rot.w, _rot.x, _rot.y, _rot.z };
	glm::mat<4, 4, float> invRoll = glm::mat4_cast(glm::angleAxis(_euler.roll * -2.0f, glm::vec3(0.0f, 1.0f, 0.0f )));
	glm::mat<4, 4, float> rotMat = glm::mat4_cast(q);

	for(auto v = vertices.begin(); v != vertices.end(); v++){
		long i = v - vertices.begin();

		vtxBuffer[i]->x = v->x;
		vtxBuffer[i]->y = v->y;
		vtxBuffer[i]->z = v->z;
		vtxBuffer[i]->w = 1;

		*vtxBuffer[i] = rotMat * *vtxBuffer[i];
		*vtxBuffer[i] = invRoll * *vtxBuffer[i];
		*vtxBuffer[i] = vpMat * *vtxBuffer[i];
		// *vtxBuffer[i] /= vtxBuffer[i]->w;
	}

	float mul = 64;
	glm::vec3 sun = glm::vec3(0, 0, 0) - glm::vec3(1, 0, 2);
	sun = glm::normalize(sun);
	for(auto f = faces.begin(); f != faces.end(); f++){
		long i = f - faces.begin();

		gvtx4* a = vtxBuffer[f->x];
		gvtx4* b = vtxBuffer[f->y];
		gvtx4* c = vtxBuffer[f->z];

		glm::vec3 _a = { a->x, a->y, 0 };
		glm::vec3 _b = { b->x, b->y, 0 };
		glm::vec3 _c = { c->x, c->y, 0 };
		glm::vec3 val = glm::cross(_b - _a, _c - _a);
		if(val.z > 0) continue;

		glm::vec3 va = { vertices[f->x].x, vertices[f->x].y, vertices[f->x].z };
		glm::vec3 vb = { vertices[f->y].x, vertices[f->y].y, vertices[f->y].z };
		glm::vec3 vc = { vertices[f->z].x, vertices[f->z].y, vertices[f->z].z };
		glm::vec3 normal = glm::cross(vb - va, vc - vb);
		normal = glm::normalize(normal);

		gvtx4 normal4 = gvtx4(normal, 0);
		normal4 = rotMat * normal4;
		normal4 = invRoll * normal4;
		//normal4 /= normal4.w;
		normal = { normal4.x, normal4.y, normal4.z };

#define RGB(R, G, B) (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3))

		float intensity = glm::dot(sun, { normal.x, normal.y, normal.z });
		intensity = max(intensity, 0.0f);
		intensity = 0.2 + 0.7 * intensity;

		canvas->fillTriangle(
				a->x * mul + 64,
				a->y * mul + 64,
				b->x * mul + 64,
				b->y * mul + 64,
				c->x * mul + 64,
				c->y * mul + 64,
				RGB(((byte) 10), ((byte) 10), ((byte)(intensity * 255)))
			);
	}

	/*canvas->setCursor(4, 2);
	canvas->printf("%3.0f fps   %3.2f ms\n", 1.0f / dt, dt * 1000.0f);
	canvas->setCursor(4, canvas->getCursorY());
	if(point != -1 ){
		canvas->printf("%3.d / 100", point);
	}

	if(point != -1){
		canvas->setCursor(12, 110);
		canvas->println("Pitch / Yaw / Roll");
		canvas->setCursor(4, canvas->getCursorY());
		canvas->printf("%.2f / %.2f / %.2f", _euler.pitch, _euler.yaw, _euler.roll);
	}*/

	wireMut->lock();
	display->commit();
	wireMut->unlock();
}

uint lastmicros = 0;

void Renderer::taskCallback(Task* task){
	Renderer* instance = static_cast<Renderer*>(task->arg);
	if(instance == nullptr) return;

	while(task->running){
		uint us = micros();
		float dt = (float) (us - lastmicros) / 1000000.0f;
		lastmicros = us;

		instance->render(dt);
	}
}

void Renderer::start(){
	renderTask.start(8 | portPRIVILEGE_BIT);
}

void Renderer::stop(){
	renderTask.stop();
}

void Renderer::setRot(const quatf& rot, const vec3f& euler){
	mut.lock();
	Renderer::rot = rot;
	Renderer::euler = euler;
	mut.unlock();
}

void Renderer::setBackground(Color background){
	this->background = background;
}

void Renderer::setPoint(uint point){
	Renderer::point = point;
}

void Renderer::incPoint(){
	point++;
}

Renderer* Renderer::getInstance(){
	return instance;
}

Mutex* Renderer::getWireMut() const{
	return wireMut;
}
