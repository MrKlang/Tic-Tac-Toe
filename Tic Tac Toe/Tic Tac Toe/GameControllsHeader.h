#pragma once
#include <utility>
#include <algorithm>

enum class Symbol
{
	empty,
	X,
	O
};

enum class Winner 
{
	Human,
	Computer,
	Tie,
	None
};

struct Field
{
	Symbol fieldSymbol;
	int index;
};

struct Player
{
	Symbol PlayerSymbol;
};

struct Move
{
	int x = 0;
	int y = 0;
	int value = (std::numeric_limits<int>::min)();
};

struct ScoreTable 
{
	int HumanScore = 0;
	int ComputerScore = 0;
};

const bool CheckIfPlayerWon(Symbol playerSymbol);
std::pair<int, Winner> UpdateGame(int index);
const bool CheckIfMoveIsPossible(int index);
void SetPlayersSymbols(Symbol chosenSymbol, Symbol computerSymbol);
void SetGameBoardIndexes();
bool IsHumanStartingGame();
void RestartGame();
int MakeFirstComputerMove();
ScoreTable GetScores();
Symbol GetHumanSymbol();
Symbol GetComputerSymbol();
void SaveScoresToFile();
void LoadScoresFromFile();