#ifndef GLYPH_H
#define GLYPH_H

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

#endif