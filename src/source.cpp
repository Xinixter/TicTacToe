#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
	std::cout << "Let's start!...";

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	GLFWwindow* sampleWindow = glfwCreateWindow(500, 500, "Sample", NULL, NULL);

	if (!sampleWindow) {
		fprintf(stderr, "Failed to create GLFW Winow\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(sampleWindow);
	glClearColor( 0.4f, 0.3f, 0.4f, 0.0f );

	while (!glfwWindowShouldClose(sampleWindow)) {

		// GLuint
		// glGenVertexArrays();
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		glfwSwapBuffers(sampleWindow);
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}
