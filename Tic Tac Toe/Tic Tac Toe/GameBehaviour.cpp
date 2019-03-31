#include "stdafx.h"
#include "GameControllsHeader.h"
#include <vector>
#include <algorithm>
#include <cmath>

const int FieldSize = 3;

class Field 
{
	public: 
		Symbol fieldSymbol;
		int index;
};

class Player 
{
	public:
		Symbol PlayerSymbol;
};

struct Move
{
	int x = 0;
	int y = 0;
	int value = (std::numeric_limits<int>::min)();
};

std::vector<std::vector<Field>> gameBoard (FieldSize,std::vector<Field>(FieldSize));
Player HumanPlayer;
Player Computer;
Move BestMove;
int RemainingFields;

void SetGameBoardIndexes() 
{
	int index = 0;
	for (int i = 0; i < FieldSize; i++) 
	{
		for (int j = 0; j < FieldSize; j++)
		{
			gameBoard[i][j].index = index;
			gameBoard[i][j].fieldSymbol = Symbol::empty;
			index++;
		}
	}

	RemainingFields = index;
}

void SetPlayersSymbols(Symbol chosenSymbol,Symbol computerSymbol) 
{
	HumanPlayer.PlayerSymbol = chosenSymbol;
	Computer.PlayerSymbol = computerSymbol;
}

const bool CheckWin(Symbol playerSymbol) 
{
	for (int i = 0; i < FieldSize;i++) 
	{
		bool row = true;
		bool column = true;

		for (int j = 0; j < FieldSize; j++) 
		{
			row = row && ( gameBoard[i][j].fieldSymbol == playerSymbol );
			column = column && ( gameBoard[j][i].fieldSymbol == playerSymbol );
		}

		if (row || column) 
		{
			return true;
		}
	}

	bool diagonal = true;

	for (int i = 0; i < FieldSize; i++) {
		diagonal = diagonal && gameBoard[i][i].fieldSymbol == playerSymbol;
	}

	if (diagonal)
	{
		return true;
	}

	diagonal = true;

	for (int i = 0; i < FieldSize;i++) 
	{
		diagonal = diagonal && ( gameBoard[FieldSize - i - 1][i].fieldSymbol == playerSymbol);
	}

	return diagonal;
}

Field * GetFieldByIndex(int index) 
{
	for (int i = 0; i < FieldSize; i++)
	{
		for (int j = 0; j < FieldSize; j++)
		{
			if (gameBoard[i][j].index == index)
			{
				return &gameBoard[i][j];
			}
		}
	}
}

const bool CheckIfMoveIsPossible(int index) 
{
	Field * temporaryField = GetFieldByIndex(index);

	return temporaryField->fieldSymbol == Symbol::empty;
}

bool isTie()
{
	return RemainingFields == 0;
}

int Search(int depth, int alpha, int beta, int scoreGiven, Player searchingPlayer, bool isMinSearch)
{
	if (CheckWin(HumanPlayer.PlayerSymbol)) 
	{ 
		return 10;
	}
	else if (CheckWin(Computer.PlayerSymbol)) 
	{ 
		return -10; 
	}
	else if (isTie()) 
	{
		return 0; 
	}

	int score = scoreGiven;

	for (unsigned i = 0; i < FieldSize; i++)
	{
		for (unsigned j = 0; j < FieldSize; j++)
		{
            if (CheckIfMoveIsPossible(gameBoard[i][j].index))
            {
                gameBoard[i][j].fieldSymbol = searchingPlayer.PlayerSymbol;
				RemainingFields--;
				
				if (isMinSearch) 
				{
					score = (std::min)(score, Search(depth + 1, alpha, beta, (std::numeric_limits<int>::min)(), HumanPlayer, false) + depth);
					beta = (std::min)(beta, score);
				}
				else 
				{
					score = (std::max)(score, Search(depth + 1, alpha, beta, (std::numeric_limits<int>::max)(), Computer, true) - depth);
					alpha = (std::max)(alpha, score);
				}

                gameBoard[i][j].fieldSymbol = Symbol::empty;
				RemainingFields++;

				if (isMinSearch) 
				{
					if (beta <= alpha) 
					{
						return beta;
					}
				}
				else 
				{
					if (beta <= alpha)
					{
						return alpha;
					}
				}
            }
        }
    }

    return score;
}

Move MinMaxAlgorithm()
{
	int depth = 0;
	bool bestMoveAlreadyChosen = false;
	bool moveNotEqualToNoMove = false;
	Move bestComputerMove;
	Move possibleMove;
	bestComputerMove.value = (std::numeric_limits<int>::max)();

	for (unsigned i = 0; i < FieldSize; i++)
	{
		for (unsigned j = 0; j < FieldSize; j++)
		{
			if (CheckIfMoveIsPossible(gameBoard[i][j].index))
			{
				gameBoard[i][j].fieldSymbol = Computer.PlayerSymbol;
				RemainingFields--;

				int bestPossibleValue = Search(depth, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), (std::numeric_limits<int>::min)(),HumanPlayer,false);

				if (bestPossibleValue < bestComputerMove.value)
				{
					bestComputerMove.value = bestPossibleValue;
					bestComputerMove.x = i;
					bestComputerMove.y = j;

					bestMoveAlreadyChosen = true;
					moveNotEqualToNoMove = true;
				}
				else if (bestPossibleValue == bestComputerMove.value) 
				{
					if (!bestMoveAlreadyChosen) 
					{
						possibleMove.value = bestPossibleValue;
						possibleMove.x = i;
						possibleMove.y = j;

						bestMoveAlreadyChosen = true;
						moveNotEqualToNoMove = false;
					}
				}

				gameBoard[i][j].fieldSymbol = Symbol::empty;
				RemainingFields++;
			}
		}
	}

	if (moveNotEqualToNoMove) 
	{
		BestMove = bestComputerMove;
	}
	else 
	{
		BestMove = possibleMove;
	}

	return BestMove;
}

int UpdateGame(int index) 
{
	GetFieldByIndex(index)->fieldSymbol = HumanPlayer.PlayerSymbol;
	RemainingFields--;

	if (CheckWin(HumanPlayer.PlayerSymbol)) 
	{
		exit(1);
	}

	if (isTie()) {
		exit(2);
	}

	Move bestMove = MinMaxAlgorithm();

	gameBoard[bestMove.x][bestMove.y].fieldSymbol = Computer.PlayerSymbol;
	RemainingFields--;

	return gameBoard[bestMove.x][bestMove.y].index;

	if (CheckWin(Computer.PlayerSymbol))
	{
		exit(0);
	}

	if (isTie()) {
		exit(2);
	}

}