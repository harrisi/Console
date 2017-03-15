// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>

int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO))
		return -1;

    return 0;
}