#ifndef PATH_H
#define PATH_H

#include "stdafx.h"
#include <string>

using namespace std;

#include "Book.h"

class path {
public:
	string codepoints;
	path();
	path(string codepoints);
	void render(float x, float y);
};

#endif