// Console.cpp : Defines the entry point for the console application.
//

// TODO: Visual Studo 2017/MS C++ does not allow this to be in a conditional
// preprocessor block.
//#include "stdafx.h"

#ifdef	__GNUG__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#include <freetype2/ft2build.h>
#else	/* __GNUG__ */
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <ft2build.h>
#endif	/* __GNUG__ */
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <iostream>
#include <exception>
using namespace std;

// TODO: Handle errors in a portable way.

// TODO: Enapsulate window and/or window configuration in a class.
// TODO: Centralize this declaration.
// TODO: Size window based on selected font size and a number of rows and
// columns.
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

class glyph {
public:
	unsigned bearing_x, bearing_y;
	// TODO: Should internal format of 1/64ths of a point be kept?
	unsigned advance_x, advance_y;
	GLuint width, height;
	GLuint texture;
	glyph();
	glyph(FT_Face face, FT_ULong codepoint);
	void render(float x, float y);
};


glyph::glyph()
{

}

// TODO: Prerendering of font cache.
// TODO: Multithreaded rendering if necessary.
glyph::glyph(FT_Face face, FT_ULong codepoint)
{
	// TODO: Allow selection of face and rendering mode.
	// TODO: Figure out why rendered glyphs appear thin with black backgrounds.
	//   Drawing the font by passing the rendered bitmap directly to OpenGL as
	// an alpha channel produces a more appealing glyph, though it is drawn in
	// black.
	FT_UInt index = FT_Get_Char_Index(face, codepoint);
	if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT)) {
		//throw exception("FT_Load_Glyph");
	}
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
		//throw exception("FT_Render_Glyph");
	}

	// TODO: Determine the best way to store glyph metrics.
	width = face->glyph->bitmap.width;
	height = face->glyph->bitmap.rows;
	bearing_x = face->glyph->bitmap_left;
	bearing_y = face->glyph->bitmap_top;
	advance_x = face->glyph->advance.x;
	advance_y = face->glyph->advance.y;

	GLubyte *bitmap = new GLubyte[width * height * 2];
	memset(bitmap, 0, width * height * 2);

	// TODO: Remove darkness around edge.
	//   Treating the glyph bitmap as an alpha channel for a white image
	// produces a better effect.
	//   Using duplicated information and glBlendEquation set to GL_MAX
	// produces good results.
	//   Glyphs still have dark artifacts on some pixels which should be
	// transparent and letting the background through. This is visible in other
	// programs, such as PowerShell and Visual Studio 2017.
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++) {
			bitmap[2 * (i + j * width) + 0] = 0xFF;
			bitmap[2 * (i + j * width) + 1] = face->glyph->bitmap.buffer[i + j * width];
		}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// OpenGL requires texture data to be aligned on 4 byte boundaries by
	// default. This removes that restriction.
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Copy data to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bitmap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
	// Set texture parameters to counteract SDL defaults that would cause the texture to not display
	// after being computed by or with e.g. an invalid mipmap.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] bitmap;
}

// TODO: Consider handling font metrics inside this function.
// TODO: Figure out how to scale fonts properly when taking into account the
// size of the screen.
void
glyph::render(float x, float y)
{
	if (!texture) {
		//throw exception("Attempt to draw unintitialized glyph.");
	}

	// TODO: Use OpenGL 4.0 vector buffer objects and vertex array objects.
	// TODO: Use shaders to provide font coloring.
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float w = 2.0f / WINDOW_WIDTH * width,
		h = 2.0f / WINDOW_HEIGHT * height;

	glTexCoord2f(0.0f, 1.0f); glVertex2f(x + 0, y + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x + 0, y + h);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + 0);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

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
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "SDL_Init" << std::endl;
		return -1;
	}

	// TODO: Use OpenGL 4 features only.
	// TODO: Better error handling.
	window = SDL_CreateWindow("Console",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
	if (!window) {
		cout << "SDL_CreateWindow: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	// TODO: Find proper event for window drawing.
	//   It is possible that it will only be necessary
	// to draw when information on the screen is updated
	// as there is no physics loop.
	if (!SDL_GL_SetSwapInterval(1)) {
		cout << "SDL_GL_SetSwapInterval" << std::endl;
		SDL_Quit();
		return -1;
	}

	context = SDL_GL_CreateContext(window);
	if (!context) {
		cout << "SDL_GL_CreateContext" << std::endl;
		SDL_Quit();
		return -1;
	}
	
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << std::endl;
	cout << "Version: " << version << std::endl;

	// Necessary to use textures.
	glEnable(GL_TEXTURE_2D);

	// TODO: Find a blending mode which eliminates the dark artifacts, as
	// suggested by a FreeType2 developer.
	// TODO: Should this be set locally when drawing glyphs?
	// Enable alpha blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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
#ifdef __GNUG__
	if (FT_New_Face(library,
		"/usr/share/fonts/ubuntu-font-family/UbuntuMono-R.ttf", 0, &face)) {
#else
	if (FT_New_Face(library, "C:\\Windows\\Fonts\\lucon.ttf", 0, &face)) {
#endif
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
