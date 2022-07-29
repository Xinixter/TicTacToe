#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <cmath>
#include <limits>
#include <utility>

#include "game.h"
#include "shader.h"


static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
static void
	MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void ProcessInput(GLFWwindow* window);

Game::Game() :
	m_Board {},
	m_Winner {},
	m_Moves {},
	m_Window { nullptr },
	m_CurrentState { GAME_INPROGRESS },
	m_GameMode { SINGLE_P },
	m_Player1Turn { true }
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

	Shader shader[2] {};
	shader[0].Load(
		"../src/assets/grid-vshader.vs", "../src/assets/grid-fshader-1.fs");

	shader[1].Load(
		"../src/assets/element-vshader.vs",
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
		// vertices                     // colors         // texture coords
		 (1.0f / 3.0f),  (1.0f / 3.0f), 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		 (1.0f / 3.0f), -(1.0f / 3.0f), 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-(1.0f / 3.0f), -(1.0f / 3.0f), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-(1.0f / 3.0f),  (1.0f / 3.0f), 1.0f, 0.0f, 1.0f, 0.0f, 1.0f  // top left
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int vao[2] {};
	glGenVertexArrays(2, vao);

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
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int elementTexture[2];
	glGenTextures(2, elementTexture);
	glBindTexture(GL_TEXTURE_2D, elementTexture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int texWidth {};
	int texHeight {};
	int nrChannels {};

	stbi_set_flip_vertically_on_load(true);

	unsigned char* xData =
		stbi_load("../src/assets/x.png", &texWidth, &texHeight, &nrChannels, 0);

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
		std::cout << "Failed to load the texture!" << std::endl;
	}

	stbi_image_free(xData);

	glBindTexture(GL_TEXTURE_2D, elementTexture[1]);

	unsigned char* oData =
		stbi_load("../src/assets/o.png", &texWidth, &texHeight, &nrChannels, 0);

	if (oData) {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texWidth,
			texHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			oData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load the texture!" << std::endl;
	}

	stbi_image_free(oData);

	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		7 * sizeof(float),
		(void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		7 * sizeof(float),
		(void*)(5 * sizeof(float)));
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

		shader[1].Use();

		for (u32 i = 0; i < 3; ++i) {
			for (u32 j = 0; j < 3; ++j) {
				if (m_Board[i][j] == 0) {
					continue;
				}

				glUniform1f(
					glGetUniformLocation(shader[1].m_SID, "deltaX"),
					static_cast<float>(j));
				glUniform1f(
					glGetUniformLocation(shader[1].m_SID, "deltaY"),
					static_cast<float>(i));

				glBindTexture(GL_TEXTURE_2D, elementTexture[m_Board[i][j] - 1]);
				glBindVertexArray(vao[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}

		if (m_CurrentState != GAME_OVER and m_GameMode == SINGLE_P and m_Player1Turn) {
			MakeMove();
			++m_Moves;
			m_Player1Turn = false;
		}

		glfwPollEvents();

		if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT)) {
			glfwGetCursorPos(m_Window, &xPos, &yPos);
			UpdateBoard(xPos, yPos);

			if (m_CurrentState == GAME_OVER) {
				if (m_Winner == 1) {
					std::cout << "Player 1 won!" << std::endl;
				}
				else if (m_Winner == -1) {
					std::cout << "Player 2 won!" << std::endl;
				}
				else {
					std::cout << "Draw!" << std::endl;
				}
				LogBoard();
			}
		}
		std::cout << m_CurrentState << std::endl;

		glfwSwapBuffers(m_Window);
	}

	return 0;
}

void Game::UpdateBoard(u32 x, u32 y)
{
	if (m_CurrentState == GAME_OVER) {
		return;
	}

	x /= 300;
	y /= 300;

	if (m_Board[y][x] == 0) {
		if (m_Player1Turn) {
			m_Board[y][x] = 1;
			m_Player1Turn = false;
		}
		else {
			m_Board[y][x] = 2;
			m_Player1Turn = true;
		}
	}
	++m_Moves;

	UpdateGameState();
}

void Game::UpdateGameState()
{
	if (m_CurrentState == GAME_OVER) {
		return;
	}

	if (m_Winner = CheckWinner(); m_Winner != Utility::T) {
		m_CurrentState = GAME_OVER;
	}
	else if (m_Moves == 9) {
		m_CurrentState = GAME_OVER;
	}
}

Utility Game::CheckWinner()
{
	for (u32 i = 0; i < 3; ++i) {
		if (m_Board[i][0] == 0 or m_Board[0][i] == 0) {
			continue;
		}

		if (m_Board[i][0] == m_Board[i][1] and m_Board[i][0] == m_Board[i][2]) {
			return m_Board[i][0] == 1 ? Utility::X : Utility::O;
		}
		if (m_Board[0][i] == m_Board[1][i] and m_Board[0][i] == m_Board[2][i]) {
			return m_Board[0][i] == 1 ? Utility::X : Utility::O;
		}
	}

	if (m_Board[0][0] != 0 and m_Board[0][0] == m_Board[1][1] and
		m_Board[0][0] == m_Board[2][2]) {
		return m_Board[0][0] == 1 ? Utility::X : Utility::O;
	}
	if (m_Board[2][0] != 0 and m_Board[2][0] == m_Board[1][1] and
		m_Board[2][0] == m_Board[0][2]) {
		return m_Board[2][0] == 1 ? Utility::X : Utility::O;
	}

	return Utility::T;
}

void Game::MakeMove()
{
	i32 bestUtilVal = std::numeric_limits<i32>::min();
	std::pair<i32, i32> currentMove {};

	for (i32 i = 0; i < 3; ++i) {
		for (i32 j = 0; j < 3; ++j) {
			if (m_Board[i][j] == 0) {
				m_Board[i][j] = 1;
				const auto utilVal = Minimax(m_Board, 0, false);
				m_Board[i][j] = 0;

				bestUtilVal = std::max(utilVal, bestUtilVal);
				currentMove.first = i;
				currentMove.second = j;
			}
		}
	}

	m_Board[currentMove.first][currentMove.second] = 1;
}

i32 Game::Minimax(i32 board[3][3], const u32& depth, const bool& isMax)
{
	if (auto temp = CheckWinner(); temp != Utility::T) {
		return temp;
	}

	i32 bestUtilVal {};
	if (isMax) {
		bestUtilVal = std::numeric_limits<i32>::min();
		for (u32 i = 0; i < 3; ++i) {
			for (u32 j = 0; j < 3; ++j) {
				if (m_Board[i][j] == 0) {
					m_Board[i][j] = 1;
					const auto utilVal = Minimax(m_Board, depth + 1, false);
					m_Board[i][j] = 0;
					bestUtilVal = std::max(utilVal, bestUtilVal);
				}
			}
		}
	}
	else {
		bestUtilVal = std::numeric_limits<i32>::max();
		for (u32 i = 0; i < 3; ++i) {
			for (u32 j = 0; j < 3; ++j) {
				if (m_Board[i][j] == 0) {
					m_Board[i][j] = 2;
					const auto utilVal = Minimax(m_Board, depth + 1, true);
					m_Board[i][j] = 0;
					bestUtilVal = std::min(utilVal, bestUtilVal);
				}
			}
		}
	}

	return bestUtilVal;
}

void Game::LogBoard()
{
	for (u32 i = 0; i < 3; ++i) {
		for (u32 j = 0; j < 3; ++j) {
			std::cout << m_Board[i][j] << ' ';
		}
		std::cout << '\n';
	}
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

static void
	MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

	if (double xPos {}, yPos {};
		button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xPos, &yPos);
		std::cout << "Left mouse button at " << xPos << ',' << yPos
				  << std::endl;
	}
}
