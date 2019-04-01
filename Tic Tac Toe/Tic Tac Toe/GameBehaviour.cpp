#include "stdafx.h"
#include "GameControllsHeader.h"
#include <vector>
#include <algorithm>
#include <cmath>

const int FieldSize = 3;

std::vector<std::vector<Field>> GameBoard (FieldSize,std::vector<Field>(FieldSize));
Winner SomeoneWonTheGame = Winner::None;
Player HumanPlayer;
Player Computer;
Move BestMove;
int RemainingFields;
bool HumanStarts = true;
ScoreTable Scores;

void SetGameBoardIndexes() 
{
	int index = 0;
	for (int i = 0; i < FieldSize; i++) 
	{
		for (int j = 0; j < FieldSize; j++)
		{
			GameBoard[i][j].index = index;
			GameBoard[i][j].fieldSymbol = Symbol::empty;
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

const bool CheckIfPlayerWon(Symbol playerSymbol) 
{
	for (int i = 0; i < FieldSize;i++) 
	{
		bool row = true;
		bool column = true;

		for (int j = 0; j < FieldSize; j++) 
		{
			row = row && ( GameBoard[i][j].fieldSymbol == playerSymbol );
			column = column && ( GameBoard[j][i].fieldSymbol == playerSymbol );
		}

		if (row || column) 
		{
			return true;
		}
	}

	bool diagonal = true;

	for (int i = 0; i < FieldSize; i++) {
		diagonal = diagonal && GameBoard[i][i].fieldSymbol == playerSymbol;
	}

	if (diagonal)
	{
		return diagonal;
	}

	diagonal = true;

	for (int i = 0; i < FieldSize;i++) 
	{
		diagonal = diagonal && ( GameBoard[FieldSize - i - 1][i].fieldSymbol == playerSymbol);
	}

	return diagonal;
}

Field * GetFieldByIndex(int index) 
{
	for (int i = 0; i < FieldSize; i++)
	{
		for (int j = 0; j < FieldSize; j++)
		{
			if (GameBoard[i][j].index == index)
			{
				return &GameBoard[i][j];
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
	if (CheckIfPlayerWon(HumanPlayer.PlayerSymbol)) 
	{ 
		return 10;
	}
	else if (CheckIfPlayerWon(Computer.PlayerSymbol)) 
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
            if (CheckIfMoveIsPossible(GameBoard[i][j].index))
            {
                GameBoard[i][j].fieldSymbol = searchingPlayer.PlayerSymbol;
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

                GameBoard[i][j].fieldSymbol = Symbol::empty;
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
			if (CheckIfMoveIsPossible(GameBoard[i][j].index))
			{
				GameBoard[i][j].fieldSymbol = Computer.PlayerSymbol;
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

				GameBoard[i][j].fieldSymbol = Symbol::empty;
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

int GetBestComputerMoveFieldIndex() 
{
	Move bestMove = MinMaxAlgorithm();
	GameBoard[bestMove.x][bestMove.y].fieldSymbol = Computer.PlayerSymbol;
	RemainingFields--;
	return GameBoard[bestMove.x][bestMove.y].index;
}

bool IsHumanStartingGame() 
{
	return HumanStarts;
}

bool SetStartingPlayer()
{
	return HumanStarts = !HumanStarts;
}

void SetWinData(Winner winner) 
{
	SetStartingPlayer();
	SomeoneWonTheGame = winner;
}

ScoreTable GetScores() 
{
	return Scores;
}

int MakeFirstComputerMove() 
{
	int index = rand()%9;
	GetFieldByIndex(index)->fieldSymbol = Computer.PlayerSymbol;
	RemainingFields--;

	return index;
}

void MakePlayerMove(int index) 
{
	GetFieldByIndex(index)->fieldSymbol = HumanPlayer.PlayerSymbol;
	RemainingFields--;
}

std::pair<int,Winner> UpdateGame(int index) 
{
	int moveIndex = -1;

	MakePlayerMove(index);

	if (CheckIfPlayerWon(HumanPlayer.PlayerSymbol)) 
	{
		SetWinData(Winner::Human);
		Scores.HumanScore++;
	}
	else if (isTie()) 
	{
		SetWinData(Winner::Tie);
	}
	else 
	{
		moveIndex = GetBestComputerMoveFieldIndex();

		if (CheckIfPlayerWon(Computer.PlayerSymbol))
		{
			SetWinData(Winner::Computer);
			Scores.ComputerScore++;
		}
		else if (isTie())
		{
			SetWinData(Winner::Tie);
		}
	}

	return { moveIndex, SomeoneWonTheGame};
}

void RestartGame() {
	SetGameBoardIndexes();
	SomeoneWonTheGame = Winner::None;
	BestMove = Move();
}