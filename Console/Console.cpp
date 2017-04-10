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

// TODO: Enapsulate window and/or window configuration in a class.
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

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
	
	float w = 2.0f / WINDOW_WIDTH * width,
		  h = 2.0f / WINDOW_HEIGHT * height;

	// TODO: Properly scale texture.
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, h);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(w, h);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(w, 0.0f);
	
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
	window = SDL_CreateWindow("Console", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
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
	// TODO: Properly render a glyph using FT_RENDER_MODE_MONO.
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
		cout << "FT_Render_Glyph" << std::endl;
		return -1;
	}

	width = face->glyph->bitmap.width;
	height = face->glyph->bitmap.rows;

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