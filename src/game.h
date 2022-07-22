#ifndef GAME_H
#define GAME_H


struct GLFWwindow;

enum GameState {
	gameMenu,
	gameInProgress,
	gameOver
};

class Game {
public:
	Game();
	~Game();

	GameState m_State;
	int Init();

private:
	GLFWwindow* m_Window;
	const unsigned int m_Width = 1000;
	const unsigned int m_Height = 900;

};

#endif