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

// TODO: Enapsulate window and/or window configuration in a class.
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// TODO: Determine if there is a better way to encapsulate the FreeType
// classes.
// TODO: Compile a display list and use vertex buffer objects.
class glyph {
public:
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
	FT_UInt index = FT_Get_Char_Index(face, codepoint);
	if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT))
		throw exception("FT_Load_Glyph");
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
		throw exception("FT_Render_Glyph");

	width = face->glyph->bitmap.width;
	height = face->glyph->bitmap.rows;

	// TODO: Determine if it's possible to only supply an alpha channel.
	GLubyte *bitmap = new GLubyte[width * height * 2];
	memset(bitmap, 0, width * height * 2);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			bitmap[2 * (i + j * width)] = bitmap[2 * (i + j * width) + 1] =
				face->glyph->bitmap.buffer[i + j * width];

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Remove requirement for a power of two sized texture?
	// TODO: Experiment with values. This does not seem to do what it claims at face value,
	// as other settings are permissible despite not making sense. On an AMD W7100 values
	// passed here seem to persist across invocations of the program.
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Copy data to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bitmap);
	// Set texture parameters to counteract SDL defaults that would cause the texture to not display
	// after being computed by or with e.g. an invalid mipmap.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] bitmap;
}

void
glyph::render(float x, float y)
{
	if (!texture)
		throw exception("Attempt to draw unintitialized glyph.");

	// TODO: Use OpenGL 4.0 vector buffer objects and vertex array objects.
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);

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

void
render(SDL_Window *window)
{
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO: Draw taking into account font metrics.
	float w = 2.0f / WINDOW_WIDTH * a.width,
		  h = 2.0f / WINDOW_HEIGHT * a.height;

	a.render(0.0f, 0.0f);
	current.render(0.0f + w, 0.0f);

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
#pragma endregion

#pragma region FreeType2
	if (FT_Init_FreeType(&library)) {
		cout << "FT_Init_Library" << std::endl;
		return -1;
	}

	// TODO: Find a cross-platform way to specify fonts.
	if (FT_New_Face(library, "C:\\Windows\\Fonts\\consola.ttf", 0, &face)) {
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
	if (FT_Set_Char_Size(face, 0, 100 * 64, hdpi, vdpi)) {
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