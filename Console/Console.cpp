// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GLFW\glfw3.h>

int
main()
{
	GLFWwindow *window;

	if (!glfwInit())
		return -1;

	if (!(window = glfwCreateWindow(640, 480, "Console", NULL, NULL))) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
    return 0;
}

