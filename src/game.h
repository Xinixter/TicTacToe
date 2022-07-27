#ifndef GAME_H
#define GAME_H


#include <cstdint>
#include <limits>

using i8  = std::int8_t;
using u8  = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

struct GLFWwindow;

enum GameState {
	GAME_MENU,
	GAME_INPROGRESS,
	GAME_OVER
};

class Game {
public:
	Game();
	~Game();

	int Init();

private:
	u32 m_Board[3][3];
	GLFWwindow* m_Window;
	GameState m_CurrentState;
	bool m_Player1Turn {};

	const u32 m_Width = 900;
	const u32 m_Height = 900;

	void UpdateGameState(unsigned int x, unsigned int y);
	bool IsOver();
	void LogBoard();

};

#endif