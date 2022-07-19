#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Game {
public:
	Game();
	~Game();

	int Start();

private:
	GLFWwindow* m_Window;

};

#endif