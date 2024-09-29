#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>
#include "MyWindow.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

float anglex;
float angley;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, static_cast<double>(WINDOW_SIZE.x / WINDOW_SIZE.y), 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 2.0, 10.0, 0.0, 2.0, 0.0, 0.0, 1.0, 0.0);

	glRotatef(anglex, 1.0f, 0.0f, 0.0f);
	glRotatef(angley, 0.0f, 1.0f, 0.0f);

	draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	draw_triangle(u8vec4(255, 0, 0, 255), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0));

	draw_triangle(u8vec4(255, 255, 0, 255), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 1.0));
	draw_triangle(u8vec4(255, 255, 0, 255), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0));

	draw_triangle(u8vec4(255, 0, 255, 255), vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	draw_triangle(u8vec4(255, 0, 255, 255), vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 0.0));

	draw_triangle(u8vec4(255, 162, 0, 255),  vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0));
	draw_triangle(u8vec4(255, 162, 0, 255),  vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0));

	draw_triangle(u8vec4(120, 0, 0, 255), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0));
	draw_triangle(u8vec4(120, 0, 0, 255), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0));

	draw_triangle(u8vec4(120, 0, 120, 255), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0));
	draw_triangle(u8vec4(120, 0, 120, 255), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0));
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