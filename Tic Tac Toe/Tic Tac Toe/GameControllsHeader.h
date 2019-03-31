#pragma once

enum class Symbol
{
	empty,
	X,
	O
};

const bool CheckWin(Symbol playerSymbol);
int UpdateGame(int index);
const bool CheckIfMoveIsPossible(int index);
void SetPlayersSymbols(Symbol chosenSymbol, Symbol computerSymbol);
void SetGameBoardIndexes();