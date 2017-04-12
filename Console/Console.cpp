// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <map>
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

// TODO: Should the drawing interface instead simply specify a glyph for each
// row and column position? (Most likely yes.)
#include "Glyph.h"
#include "Path.h"

// TODO: Enapsulate window and/or window configuration in a class.
// TODO: Centralize this declaration.
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// TODO: Encapsulate in a class.
GLuint width, height;
GLuint texture;
glyph a, current;

string initial_glyphs = "abcdefghijklmnopqrstuvwxyz0123456789";
map<string, glyph> book;

void
render(SDL_Window *window)
{
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO: Draw taking into account font metrics.
	float x = 0.0f + 2.0f / WINDOW_WIDTH * (current.bearing_x), // bearing x.
		  y = 0.0f - 2.0f / WINDOW_HEIGHT * (current.height - current.bearing_y); // height - bearing y.
	float w = 2.0f / WINDOW_WIDTH * (a.advance_x / 64), // this seems to be too far.
		  h = 2.0f / WINDOW_HEIGHT * (a.advance_y / 64);

	// Origin of 0, 0. Texture origin may be below and to the right of this
	// point.

	a.render(0.0f, 0.0f);
	current.render(x + w, y);

	SDL_GL_SwapWindow(window);
}

int
main(int argc, char *argv[])
{
	// TODO: Better error handling.
	// TODO: Put FreeType code in a class.
	FT_Library library;
	FT_Face face;
	FT_UInt index;

	// TODO: Titleless window.
	// TODO: Hotkeys for movement - window click and drag, etc.
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;
	int displays;
	float ddpi, vdpi, hdpi;

#pragma region SDL2
	if (SDL_Init(SDL_INIT_VIDEO))
		return -1;

	// TODO: Use OpenGL 4 features only.
	// TODO: Better error handling.
	window = SDL_CreateWindow("Console",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
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
	
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << std::endl;
	cout << "Version: " << version << std::endl;

	// Necessary to use textures.
	glEnable(GL_TEXTURE_2D);
	// Enable alpha blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// TODO: Find an extensions loading library or properly load extensions.
	//wglGetProcAddress("wglGetExtensionsStringARB");
	//((void (*)(int))wglGetProcAddress("glBlendEquation"))(GL_MAX);
#pragma endregion

#pragma region FreeType2
	if (FT_Init_FreeType(&library)) {
		cout << "FT_Init_Library" << std::endl;
		return -1;
	}

	// TODO: Find a cross-platform way to specify fonts.
	// TODO: Get a list of suggested fonts. Consider Consolas, Lucidia Console.
	//   consola.ttf, lucon.ttf.
	if (FT_New_Face(library, "C:\\Windows\\Fonts\\lucon.ttf", 0, &face)) {
		cout << "FT_New_Face" << std::endl;
		return -1;
	}

	// TODO: Support multiple monitors and/or monitors with disparate metrics.
	//   SDL seems to have some transparent support for HiDPI displays, figure
	//   out how to use it if possible. How to handle the case where the window
	//   spans multiple monitors?
	if ((displays = SDL_GetNumVideoDisplays()) < 0) {
		cout << "SDL_GetNumVideoDisplays" << std::endl;
		return -1;
	}
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)) {
		cout << "SDL_GetDisplayDPI" << std::endl;
		return -1;
	}
	if (FT_Set_Char_Size(face, 0, 9 * 64, hdpi, vdpi)) {
		cout << "FT_Set_Char_Size" << std::endl;
		return -1;
	}
	
	a = glyph(face, (FT_ULong)'a');
	current = a;
#pragma endregion FreeType2

#pragma region EventLoop
	// TODO: Find a cross-platform way to handle unicode input. It seems to
	// have been removed from SDL around the 1.3 release.
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

			current = glyph(face, (FT_ULong)event.key.keysym.sym);

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
#pragma endregion

	return 0;
}