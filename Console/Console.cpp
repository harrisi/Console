// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GLFW\glfw3.h>

void
refresh(GLFWwindow *window)
{
	printf("Hello, world!\n");
	
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	return;
}

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
	glClearColor(0.1f, 0.2f, 0.5f, 0.0f);

	glfwSetWindowRefreshCallback(window, refresh);

	// TODO: Find out how to enable VSync.
	while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT);
		//glfwSwapBuffers(window);
		glfwWaitEvents();
	}

	glfwTerminate();
    return 0;
}

