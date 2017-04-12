#include "stdafx.h"
#include "Glyph.h"

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
	if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT))
		throw exception("FT_Load_Glyph");
	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
		throw exception("FT_Render_Glyph");

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
	if (!texture)
		throw exception("Attempt to draw unintitialized glyph.");

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
