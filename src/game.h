#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Game {
public:
	Game();
	~Game();

	int Init();

private:
	GLFWwindow* m_Window;
	const unsigned int m_Width = 1000;
	const unsigned int m_Height = 900;

};

#endif