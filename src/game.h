#ifndef GAME_H
#define GAME_H


#include <cstdint>
#include <limits>


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

enum GameMode {
	SINGLE_P,
	MULTI_P
};

enum Utility {
	O = -1,
	T,
	X
};

class Game {
public:
	Game();
	~Game();

	int Init();

private:
	i32         m_Board[3][3];
	u32         m_Moves;
	Utility     m_Winner;
	GLFWwindow* m_Window;
	GameState   m_CurrentState;
	GameMode    m_GameMode;
	bool        m_Player1Turn {};

	const u32   m_Width  = 900;
	const u32   m_Height = 900;

	void        UpdateBoard(u32 x, u32 y);
	void        UpdateGameState();
	Utility     CheckWinner();
	void        MakeMove();
	i32         Minimax(i32 board[3][3], const u32& depth, const  bool& isMax);

	void LogBoard();
};

#endif