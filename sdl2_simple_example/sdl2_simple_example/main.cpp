#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_events.h>
#include "MyWindow.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;
using mat4 = glm::dmat4;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

float anglex;
float angley;

class GraphicObject {
	mat4 _mat = glm::identity<mat4>();

public:
	const mat4& mat() { return _mat; }
	vec3& pos() { return *(vec3*)&_mat[3]; }
	const vec3& fwd() { return *(vec3*)&_mat[2]; }
	const vec3& up() { return *(vec3*)&_mat[1]; }
	const vec3& left() { return *(vec3*)&_mat[0]; }

	void translate(const vec3& v) { _mat = glm::translate(_mat, v);	}
	void rotate(double angle, const vec3& axis) { _mat = glm::rotate(_mat, angle, axis); }

	virtual void paint() = 0;

};

struct Camera : public GraphicObject {
	vec3 target() { return pos() + fwd(); }

	double fov = glm::radians(60.0);
	double zNear = 0.1;
	double zFar = 100.0;

	double aspect() { return static_cast<double>(WINDOW_SIZE.x) / WINDOW_SIZE.y; }

	void paint() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		mat4 proj_mat = glm::perspective(fov, aspect(), zNear, zFar);
		glLoadMatrixd(&proj_mat[0][0]);

		glMatrixMode(GL_MODELVIEW);
		mat4 view_mat = glm::lookAt(pos(), target(), up());
		glLoadMatrixd(&view_mat[0][0]);
	}
};

struct Triangle : public GraphicObject {
	u8vec4 color = u8vec4(255,0,0,255);
	double size = 0.5;
	vec3 p = pos();
	void paint() override {

		glMultMatrixd(&mat()[0][0]);

		glColor4ub(color.r, color.g, color.b, color.a);
		glBegin(GL_TRIANGLES);
		glVertex3d(p.x - size, p.y - size, p.z + size);
		glVertex3d(p.x + size, p.y - size, p.z + size);
		glVertex3d(p.x + size, p.y + size, p.z + size);

		glVertex3d(p.x + size, p.y + size, p.z + size);
		glVertex3d(p.x - size, p.y + size, p.z + size);
		glVertex3d(p.x - size, p.y - size, p.z + size);

		// Back face
		glVertex3d(p.x - size, p.y - size, p.z - size);
		glVertex3d(p.x - size, p.y + size, p.z - size);
		glVertex3d(p.x + size, p.y + size, p.z - size);

		glVertex3d(p.x + size, p.y + size, p.z - size);
		glVertex3d(p.x + size, p.y - size, p.z - size);
		glVertex3d(p.x - size, p.y - size, p.z - size);

		// Left face
		glVertex3d(p.x - size, p.y - size, p.z - size);
		glVertex3d(p.x - size, p.y - size, p.z + size);
		glVertex3d(p.x - size, p.y + size, p.z + size);

		glVertex3d(p.x - size, p.y + size, p.z + size);
		glVertex3d(p.x - size, p.y + size, p.z - size);
		glVertex3d(p.x - size, p.y - size, p.z - size);

		// Right face
		glVertex3d(p.x + size, p.y - size, p.z - size);
		glVertex3d(p.x + size, p.y + size, p.z - size);
		glVertex3d(p.x + size, p.y + size, p.z + size);

		glVertex3d(p.x + size, p.y + size, p.z + size);
		glVertex3d(p.x + size, p.y - size, p.z + size);
		glVertex3d(p.x + size, p.y - size, p.z - size);

		// Top face
		glVertex3d(p.x - size, p.y + size, p.z - size);
		glVertex3d(p.x - size, p.y + size, p.z + size);
		glVertex3d(p.x + size, p.y + size, p.z + size);

		glVertex3d(p.x + size, p.y + size, p.z + size);
		glVertex3d(p.x + size, p.y + size, p.z - size);
		glVertex3d(p.x - size, p.y + size, p.z - size);

		// Bottom face
		glVertex3d(p.x - size, p.y - size, p.z - size);
		glVertex3d(p.x + size, p.y - size, p.z - size);
		glVertex3d(p.x + size, p.y - size, p.z + size);

		glVertex3d(p.x + size, p.y - size, p.z + size);
		glVertex3d(p.x - size, p.y - size, p.z + size);
		glVertex3d(p.x - size, p.y - size, p.z - size);
		glEnd();
	}
};



static Camera camera;
static Triangle triangle;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	camera.pos() = vec3(0.0, 0.0, 5.0);
	camera.rotate(glm::radians(180.0), camera.up());
}

static void draw_triangle(const u8vec4& color, const vec3& firstvert, const vec3& secondvert, const vec3& thirdvert) {
	glColor4ub(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	glVertex3d(firstvert.x, firstvert.y , firstvert.z);
	glVertex3d(secondvert.x, secondvert.y , secondvert.z);
	glVertex3d(thirdvert.x, thirdvert.y, thirdvert.z);
	glEnd();
}

static void display_func() {
	
	camera.paint();
	triangle.paint();
	//glRotatef(anglex, 1.0f, 0.0f, 1.0f);
	//glRotatef(angley, 0.0f, 1.0f, 0.0f);

	//draw_triangle(u8vec4(255,    0, 0, 255), vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	//draw_triangle(u8vec4(255,    0, 0, 255), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0));
	//
	//draw_triangle(u8vec4(255,  255, 0, 255), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 1.0));
	//draw_triangle(u8vec4(255,  255, 0, 255), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0));
	//
	//draw_triangle(u8vec4(255,  0, 255, 255), vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//draw_triangle(u8vec4(255,  0, 255, 255), vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 0.0));
	//
	//draw_triangle(u8vec4(255, 162, 0, 255),  vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0));
	//draw_triangle(u8vec4(255, 162, 0, 255),  vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0));
	//
	//draw_triangle(u8vec4(120,    0, 0, 255), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0));
	//draw_triangle(u8vec4(120,    0, 0, 255), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0));
	//						  
	//draw_triangle(u8vec4(120,  0, 120, 255), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0));
	//draw_triangle(u8vec4(120,  0, 120, 255), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0));
}

static bool processEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

	init_openGL();

	while (processEvents()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const auto t0 = hrclock::now();
		display_func();
		anglex++;
		angley++;
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}


	return 0;
}