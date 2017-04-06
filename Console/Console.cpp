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

// TODO: Find a more compact and faster implementation.
#define __LOG2A(s) ((s &0xffffffff00000000) ? (32 +__LOG2B(s >>32)): (__LOG2B(s)))
#define __LOG2B(s) ((s &0xffff0000)         ? (16 +__LOG2C(s >>16)): (__LOG2C(s)))
#define __LOG2C(s) ((s &0xff00)             ? (8  +__LOG2D(s >>8)) : (__LOG2D(s)))
#define __LOG2D(s) ((s &0xf0)               ? (4  +__LOG2E(s >>4)) : (__LOG2E(s)))
#define __LOG2E(s) ((s &0xc)                ? (2  +__LOG2F(s >>2)) : (__LOG2F(s)))
#define __LOG2F(s) ((s &0x2)                ? (1)                  : (0))

#define LOG2_UINT64 __LOG2A
#define LOG2_UINT32 __LOG2B
#define LOG2_UINT16 __LOG2C
#define LOG2_UINT8  __LOG2D

static inline uint64_t
next_pow2(uint64_t i)
{
#if defined(__GNUC__)
	return 1UL << (1 + (63 - __builtin_clzl(i - 1)));
#else
	i = i - 1;
	i = LOG2_UINT64(i);
	return 1UL << (1 + i);
#endif
}

// TODO: Encapsulate in a class.
GLuint width, height;
GLuint texture;

void
render(SDL_Window *window)
{
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO: Use OpenGL 4.0 vector buffer objects and vertex array objects.
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	
	// TODO: Properly scale texture.
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 0.0f);
	
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

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

	// TODO: Titleless window.
	// TODO: Hotkeys for movement - window click and drag, etc.
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;

#pragma region SDL2
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

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("Version: %s\n", version);

	// Necessary to use textures.
	glEnable(GL_TEXTURE_2D);
#pragma endregion

#pragma region FreeType2
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
	//   FT_REDNER_MODE_MONO for monochromatic.
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
		cout << "FT_Render_Glyph" << std::endl;
		return -1;
	}

	width = next_pow2(face->glyph->bitmap.width);
	height = next_pow2(face->glyph->bitmap.rows);

	cout << "Width: " << face->glyph->bitmap.width << ", " << width << std::endl;
	cout << "Height: " << face->glyph->bitmap.rows << ", " << height << std::endl;

	GLubyte *bitmap = new GLubyte[width * height * 2];
	memset(bitmap, 0, width * height * 2);

	// TODO: Fix image format.
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			bitmap[2 * (i + j * width)] = bitmap[2 * (i + j * width) + 1] = 
				(i >= width || j >= height) ? 0 : face->glyph->bitmap.buffer[i + j * width];

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Copy data to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bitmap);

	// Set texture parameters to counteract SDL defaults that would cause the texture to not display
	// after being computed by or with e.g. an invalid mipmap.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// TODO: Look at other values - resizing is potentially more desireable.
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#pragma endregion FreeType2

#pragma region EventLoop
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
#pragma endregion

	return 0;
}