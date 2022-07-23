#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "game.h"
#include "shader.h"


static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void ProcessInput(GLFWwindow* window);

Game::Game() : m_Window { nullptr }
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	std::cout << "Initiated" << std::endl;

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

Game::~Game()
{
	glfwTerminate();
}

int Game::Init()
{
	m_Window = glfwCreateWindow(m_Width, m_Height, "TicTacToe", NULL, NULL);
	if (m_Window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, 1);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);

	const char* vertexShaderSource = R"GLSL(
		#version 330 core
		layout (location = 0) in vec3 posA;
		void main()
		{
			gl_Position = vec4(posA.x, posA.y, posA.z, 1.0);
		}
	)GLSL";

	const char* fragmentShaderSource = R"GLSL(
		#version 330 core
		out vec4 fragColor;
		void main()
		{
			fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		}
	)GLSL";

	Shader shader {};
	shader.Compile(vertexShaderSource,fragmentShaderSource);

	float board[] = {
		-(1.0f/3.0f),  1.0f, 0.0f,
		-(1.0f/3.0f), -1.0f, 0.0f,
		 (1.0f/3.0f),  1.0f, 0.0f,
		 (1.0f/3.0f), -1.0f, 0.0f,
		-1.0f, -(1.0f/3.0f), 0.0f,
		 0.9f, -(1.0f/3.0f), 0.0f,
		-1.0f,  (1.0f/3.0f), 0.0f,
		 0.9f,  (1.0f/3.0f), 0.0f
	};

	float tempTriangle[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	unsigned int vao[2] {};
	glGenVertexArrays(2, vao);
	glBindVertexArray(vao[1]);

	unsigned int vbo[2] {};
	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(board), board, GL_STATIC_DRAW);


	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);    // Unbinding buffer
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(m_Window)) {
		ProcessInput(m_Window);

		glClearColor(0.102f, 0.102f, 0.102f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(vao[1]);
		glDrawArrays(GL_LINES, 0, 8);

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	return 0;
}

static void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

static void FrameBufferSizeCallback(
	GLFWwindow* window,
	const int   width,
	const int   height)
{
	glViewport(0, 0, width, height);
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

	if (double xPos {}, yPos {};
		button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xPos, &yPos);
		std::cout << "Left mouse button at " << xPos << ',' << yPos
				  << std::endl;
	}
}
