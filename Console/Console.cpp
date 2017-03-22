// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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
	// TODO: Create instructions for building dependencies on Windows.
	//   Currently, binaries are copied from build directory into the submodule directory.
	//   Find a way to automatically build dependencies.
	// TODO: Put FreeType code in a class.
	FT_Library library;
	FT_Face face;
	FT_UInt index;

	if (FT_Init_FreeType(&library)) {
		cout << "FT_Init_Library" << std::endl;
		return -1;
	}

	if (FT_New_Face(library, "C:\\Windows\\Fonts\\consola.ttf", 0, &face)) {
		cout << "FT_New_Face" << std::endl;
		return -1;
	}

	// TODO: Make this compatible with HiDPI screens.
	//   Get DPI from SDL, calculate character height/width in 1/64ths of a point.
	//   Horizontal and vertical DPI is last two arguments.
	if (FT_Set_Char_Size(face, 0, 16 * 64, 300, 300)) {
		cout << "FT_Set_Char_Size" << std::endl;
		return -1;
	}

	// TODO: Keep in mind proper unicode processing when obtaining values to
	// pass to this function.
	// TODO: How to handle failed glyph lookups at runtime?
	index = FT_Get_Char_Index(face, (FT_ULong)'a');
	if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT)) {
		cout << "FT_Load_Glyph" << std::endl;
		return -1;
	}

	// TODO: Allow selection of render mode.
	//   FT_RENDER_MODE_NORMAL for antialiasing.
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO)) {
		cout << "FT_Render_Glyph" << std::endl;
		return -1;
	}

	// TODO: Copy glyph bitmap into texture.

	// TODO: Titleless window.
	// TODO: Hotkeys for movement - window click and drag, etc.
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
		render(window);

		switch (event.type) {
		case SDL_QUIT: {
			SDL_Quit();
			break;
		}
		case SDL_WINDOWEVENT: {
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.repeat)
				continue;

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

	return 0;
}