// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>
#include <SDL_opengl.h>

int
main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO))
		return -1;

	window = SDL_CreateWindow("Console", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_Quit();
		return -1;
	}

	// TODO: Use OpenGL 4 features only.
	// TODO: Better error handling.
	//if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
	//	return -2;
	//if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
	//	return -3;
	//if (!SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1))
	//	return -4;
	//if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3))
	//	return -5;
	//if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5))
	//	return -6;

	if (!SDL_GL_SetSwapInterval(1)) {
		SDL_Quit();
		return -1;
	}

	context = SDL_GL_CreateContext(window);
	if (!context) {
		SDL_Quit();
		return -1;
	}

	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);

	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_QUIT) {
			SDL_Quit();
			return 0;
		}

		if (event.type == SDL_WINDOWEVENT) {
			switch (event.window.event) {

			}
		}
	}
}