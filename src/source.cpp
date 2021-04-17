#include <iostream>

#include <GLFW/glfw3.h>

int main()
{
	std::cout << "Let's start!...";

	GLFWwindow* sampleWindow;

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	sampleWindow = glfwCreateWindow(500, 500, "Sample", NULL, NULL);

	if (!sampleWindow) {
		fprintf(stderr, "Failed to create GLFW Winow\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	while (!glfwWindowShouldClose(sampleWindow))
	{
		glfwSwapBuffers(sampleWindow);
		glfwPollEvents();
		
	}
	
	glfwTerminate();

	return 0;
}
