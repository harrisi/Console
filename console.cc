// Console.cpp : Defines the entry point for the console application.
//

// TODO: Visual Studo 2017/MS C++ does not allow this to be in a conditional
// preprocessor block.
//#include "stdafx.h"

// TODO: Search for better and more exclusive macros. Possibly configure CMake
// to avoid include location variance.
#if	defined(_MSC_VER) || defined(__MINGW32__)
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <ft2build.h>
#else	/* __GNUG__ */
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#include <freetype2/ft2build.h>
#endif
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
// TODO: Window size in pixels needs to be recovered from
// SDL_GL_GetDrawableSize as some OSes will hide high density display
// capability.
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480
#define SCREEN_ROWS		10
#define SCREEN_COLS		10

class glyph {
public:
	int bearing_x, bearing_y;
	// TODO: Should internal format of 1/64ths of a point be kept?
	// TODO: Make sure all math involving font positioning is floating point.
	int advance_x, advance_y;
	// TODO: Find a better place to put this.
	int descender;
	int width, height;
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
		cout << "FT_Load_Glyph" << std::endl;
	}
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
		cout << "FT_Render_Glyph" << std::endl;
	}

	// TODO: Determine the best way to store glyph metrics.
	width = face->glyph->bitmap.width;
	height = face->glyph->bitmap.rows;
	bearing_x = face->glyph->bitmap_left;
	bearing_y = face->glyph->bitmap_top;
	advance_x = face->glyph->advance.x;
	advance_y = face->glyph->advance.y;
	descender = face->descender;

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
	for (unsigned i = 0; i < width; i++)
		for (unsigned j = 0; j < height; j++) {
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

// TODO: Make these non-global.
// TODO: Better error handling.
// TODO: Put FreeType code in a class.
FT_Library library;
FT_Face face;

// TODO: Encapsulate in a class.
// TODO: Create variables for orthographic projection limits.
int window_width, window_height, displays;
float ddpi, vdpi, hdpi;
GLuint texture;

// TODO: Proper glyph caching.
string initial_glyphs = "abcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()`~-_=+[{]}\\|;:'\",<.>/?";
// TODO: Make this unicode-aware. It was map<string, glyph>.
map<string, glyph> book;
string *screen;
int screen_width, screen_height;

// TODO: Create the font cache by iterating through all glyphs in the file and
// record the largest y bearing for use in positioning the origins of glyphs.
// TODO: Fix the display of ' and " and possibly more glyphs.

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

	x = x;
	y = y + (1.0f / window_height * (0.0f));

	float w = 1.0f / window_width * width,
		  h = 1.0f / window_height * height;

	glTexCoord2f(0.0f, 1.0f); glVertex2f(x + 0, y + 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x + 0, y + h);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + 0);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

// TODO: Create other functions as appropriate to size the main window based on
// selected font face.
void
get_font_metrics(FT_Face *face)
{
	// face->bbox: smallest box that contains all glyphs in face.
	// face->height: value to use for linefeed.
	// face->max_advance_width: consider for use instead of bbox values.
	// face->max_advance_height: consider for use instead of bbox or height
	//   values.
}

// TODO: Determine why this lags the display on Linux and OSX. It may be GPU
// capability related.
void
render(SDL_Window *window)
{
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	string a = screen[0 + SCREEN_COLS * 0],
		   b = screen[1 + SCREEN_COLS * 0];

	if (book.find(b) == book.end())
		book[b] = glyph(face, (FT_ULong)b.at(0));

	glyph c = book[a],
		  d = book[b];

	// The first element is at the bottom left.
	float base_x = 0.0f,
		  base_y = 0.0f;

	// TODO: Invalid glyphs are drawn.
	// TODO: All glyphs move with the baseline of the second.
	for (int i = 0; i < SCREEN_COLS; i++) {
		for (int j = 0; j < SCREEN_ROWS; j++) {
			glyph g = book[screen[j + SCREEN_COLS * i]];
			float x = base_x + 1.0f / window_width * (g.bearing_x),
				  y = base_y + (1.0f / window_height * (-(int)g.descender / 64.0f)) -
					  1.0f / window_height * (g.height - g.bearing_y),
				  w = 1.0f / window_width * (g.advance_x / 64.0f),
				  h = 1.0f / window_height * (g.advance_y / 64.0f);
			
			g.render(x, y);
			base_x += w;
		}

		base_x  = 0.0f;
		// TODO: Determine why bbox seems to be twice the actual value.
		base_y += 1.0f / window_height * (face->max_advance_height / 64.0f / 2.0f);
	}

	// TODO: There are artifacts drawn when using large sizes.
	// TODO: face->descender is not the proper value to use.
	// TODO: Calculate these positions assuming a grid of monospaced glyphs.
	// TODO: Move this into the glyph class or find a better way to centralize it.
	// TODO: Draw taking into account font metrics.
	// TODO: Glyphs on Windows are raised up farther than on Linux.

	SDL_GL_SwapWindow(window);
}

int
main(int argc, char *argv[])
{
	// TODO: Titleless window.
	// TODO: Hotkeys for movement - window click and drag, etc.
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "SDL_Init" << std::endl;
		return -1;
	}

	if (FT_Init_FreeType(&library)) {
		cout << "FT_Init_Library" << std::endl;
		return -1;
	}

	// TODO: Find a cross-platform way to specify fonts.
	// TODO: Get a list of suggested fonts. Consider Consolas, Lucidia Console.
	//   consola.ttf, lucon.ttf.
#if defined(__APPLE__)
	if (FT_New_Face(library, "/System/Library/Fonts/Monaco.dfont", 0, &face)) {
#elif defined(__GNUG__)
	if (FT_New_Face(library,
		"/usr/share/fonts/ubuntu-font-family/UbuntuMono-R.ttf", 0, &face)) {
#else // WINDOWS
	if (FT_New_Face(library, "C:\\Windows\\Fonts\\lucon.ttf", 0, &face)) {
#endif
		cout << "FT_New_Face" << std::endl;
		return -1;
	}

	// TODO: Support multiple monitors and/or monitors with disparate metrics.
	//   SDL seems to have some transparent support for HiDPI displays, figure
	//   out how to use it if possible. How to handle the case where the window
	//   spans multiple monitors?
	// TODO: Program displays at reasonable size on OSX but does not display at
	//   the correct resolution.
	// Sometimes the glyph textures will have blending artifacts on OSX.
	if ((displays = SDL_GetNumVideoDisplays()) < 0) {
		cout << "SDL_GetNumVideoDisplays" << std::endl;
		return -1;
	}
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)) {
		cout << "SDL_GetDisplayDPI" << std::endl;
		return -1;
	}
	// TODO: Windows and MacOS points seem to differ from FreeType2's point.
	if (FT_Set_Char_Size(face, 0, 12 * 64, (FT_UInt)hdpi, (FT_UInt)vdpi)) {
		cout << "FT_Set_Char_Size" << std::endl;
		return -1;
	}

	// TODO: Figure out why face->bbox values are twice as large as they
	// should be.
	cout << face->bbox.xMax / 64.0f << std::endl;
	cout << face->bbox.yMax / 64.0f << std::endl;

	// TODO: Find why the window is sized improperly.
	//   Using the values in the bounding box results in a closer size.
	// TODO: Properly handle empty (zero) memory locations. Should they be
	// spaces?
	screen_width = (int)((face->max_advance_width / 64.0f) * SCREEN_COLS);
	screen_height = (int)((face->bbox.yMax / 64.0f) * SCREEN_ROWS);
	screen = new string[SCREEN_COLS * SCREEN_ROWS];
	//memset(screen, 0, SCREEN_COLS * SCREEN_ROWS);
	for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
		screen[i] = "";
	screen[0 + SCREEN_COLS * 0] = "a";

	cout << screen_width << std::endl;
	cout << screen_height << std::endl;
	
	// TODO: Use OpenGL 4 features only.
	// TODO: Better error handling.
	window = SDL_CreateWindow("Console",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
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

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << std::endl;
	cout << "Version: " << version << std::endl;

	// TODO: Verify this works.
	//   Per testing on OSX this seems to be functional.
	// Some OSes will automatically scale windows when using a high DPI screen.
	// This should undo that.
	SDL_GL_GetDrawableSize(window, &window_width, &window_height);
	cout << "Width: " << window_width << std::endl;
	cout << "Height: " << window_height << std::endl;

	glViewport(0, 0, window_width, window_height);
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

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

	// TODO: Compartmentalize.
	// TODO: Support UTF-8.
	//   Unfortunately it is not possible to iterate through all glyph indices
	// and recover their code points.
	for (auto &c : initial_glyphs) {
		book["" + c] = glyph(face, (FT_ULong)c);
	}

	for (auto &g : book)
		cout << g.first << std::endl;

#pragma region EventLoop
	// TODO: All input is unicode input.
	SDL_StartTextInput();
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
		case SDL_TEXTINPUT: {
			string text(event.text.text);
			cout << text << std::endl;
			screen[1 + screen_width * 0] = text;
		}
		case SDL_TEXTEDITING: {
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

	SDL_StopTextInput();
#pragma endregion

	return 0;
}
