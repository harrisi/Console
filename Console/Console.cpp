// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <ft2build.h>

using namespace std;

class glyph {
public:
	int texture_id;
	glyph();
	void from_string(string codepoint);
};

glyph::glyph()
{

}

void
glyph::from_string(string codepoint)
{

}

void
render(SDL_Window *window)
{
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
}

int
main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO))
		return -1;

	// TODO: Use OpenGL 4 features only.
	// TODO: Better error handling.
	window = SDL_CreateWindow("Console", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_Quit();
		return -1;
	}

	// TODO: Find proper event for window drawing.
	//   It is possible that it will only be necessary
	// to draw when information on the screen is updated
	// as there is no physics loop.
	if (!SDL_GL_SetSwapInterval(1)) {
		SDL_Quit();
		return -1;
	}

	context = SDL_GL_CreateContext(window);
	if (!context) {
		SDL_Quit();
		return -1;
	}

	// TODO: Better event handling mechanism.
	// TODO: Create a window and game logic class?
	while (SDL_WaitEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {
			break;
		}
		case SDL_WINDOWEVENT: {
			switch (event.window.event) {
			}
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.repeat)
				continue;

			cout << event.key.keysym.sym << std::endl;
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: {
				SDL_Quit();
				return 0;
			}
			}
			break;
		}
		}
	}
}