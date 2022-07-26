#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "game.h"
#include "shader.h"
#include "stb_image.h"


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

	// const auto& vertexPath = std::filesystem::path("./assets/basic-vshader.vs").string();
	// const char* vertexShaderSource = vertexPath.c_str();

	// const auto& fragPath1 = std::filesystem::path("./assets/frag-vshader-1.fs").string();
	// const char* fragmentShaderSource = fragPath1.c_str();

	// const auto& fragPath2 = std::filesystem::path("./assets/frag-shader-2.fs").string();
	// const char* fragShaderSource = fragPath2.c_str();

	Shader shader[2] {};
	shader[0].Load("../src/assets/grid-vshader.vs",
				   "../src/assets/grid-fshader-1.fs");

	shader[1].Load("../src/assets/element-vshader.vs",
				   "../src/assets/element-fshader-2.fs");

	float board[] = {
		-(1.0f/3.0f),  1.0f, 0.0f,
		-(1.0f/3.0f), -1.0f, 0.0f,
		 (1.0f/3.0f),  1.0f, 0.0f,
		 (1.0f/3.0f), -1.0f, 0.0f,
		-1.0f, -(1.0f/3.0f), 0.0f,
		 1.0f, -(1.0f/3.0f), 0.0f,
		-1.0f,  (1.0f/3.0f), 0.0f,
		 1.0f,  (1.0f/3.0f), 0.0f
	};

	float xo[] = {
		// vertices                       // colors         // texture coords
		 (1.0f/3.0f),  (1.0f/3.0f), 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		 (1.0f/3.0f), -(1.0f/3.0f), 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-(1.0f/3.0f), -(1.0f/3.0f), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-(1.0f/3.0f),  (1.0f/3.0f), 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f  // top left
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	unsigned int vao[2] {};
	glGenVertexArrays(2, vao);
	glBindVertexArray(vao[1]);

	unsigned int vbo[2] {};
	glGenBuffers(2, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(board), board, GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xo), xo, GL_STATIC_DRAW);

	unsigned int ebo {};
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int elementTexture;
	glGenTextures(1, &elementTexture);
	glBindTexture(GL_TEXTURE_2D, elementTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int texWidth {};
	int texHeight {};
	int nrChannels {};

	stbi_set_flip_vertically_on_load(true);

	unsigned char* xData =
		stbi_load("../src/assets/x-alt.png", &texWidth, &texHeight, &nrChannels, 0);
	// unsigned char* oData = stbi_load("../src/assets/o.png", &texWidth, &texHeight, &nrChannels, 0);
	
	if (xData) {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texWidth,
			texHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			xData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to Load the texture!" << std::endl;
	}

	// glBindTexture(GL_TEXTURE_2D, elementTexture[1]);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth[1], texHeight[1], 0, GL_RGB, GL_UNSIGNED_BYTE, oData);
	// glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(xData);

	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);    // Unbinding buffer
	glBindVertexArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double xPos {};
	double yPos {};

	while (!glfwWindowShouldClose(m_Window)) {
		ProcessInput(m_Window);

		glClearColor(0.102f, 0.102f, 0.102f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader[0].Use();
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_LINES, 0, 8);

		glfwPollEvents();
		glfwGetCursorPos(m_Window, &xPos, &yPos);

		if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) and
			xPos >= 300 and xPos <= 600 and yPos >= 300 and yPos <= 600) {
			shader[1].Use();
			glBindTexture(GL_TEXTURE_2D, elementTexture);
			glBindVertexArray(vao[1]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(m_Window);
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
