#ifndef BOOK_H
#define BOOK_H

#include "stdafx.h"
#include <map>

using namespace std;

#include "Glyph.h"

// TODO: Initialization of this class needs to be unicode aware.
class book {
public:
	map<string, glyph> glyphs;
	book(FT_Face face);
	book(FT_Face face, string codepoints);
};

#endif