#include "stdafx.h"
#include "GameControllsHeader.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <regex>

const int FieldSize = 3;

std::vector<std::vector<Field>> GameBoard (FieldSize,std::vector<Field>(FieldSize));
Winner SomeoneWonTheGame = Winner::None;
Player HumanPlayer;
Player Computer;
Move BestMove;
int RemainingFields;
bool HumanStarts = true;
ScoreTable Scores;

// Funkcja przypisuj¹ca indeksy i pusty symbol do komórek planszy
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

// Funkcja przypisuj¹ca symbole graczom 
void SetPlayersSymbols(Symbol chosenSymbol,Symbol computerSymbol) 
{
	HumanPlayer.PlayerSymbol = chosenSymbol;
	Computer.PlayerSymbol = computerSymbol;
}

// Funkcja sprawdzaj¹ca czy dany gracz wygra³ (po kolei rzêdy i kolumny a potem skosy)
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

// Funkcja zwracaj¹ca pole o danym indeksie
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

// Funkcja sprawdzaj¹ca poprawnoœæ ruchu
const bool CheckIfMoveIsPossible(int index) 
{
	Field * temporaryField = GetFieldByIndex(index);

	return temporaryField->fieldSymbol == Symbol::empty;
}

// Funkcja sprawdzaj¹ca czy nast¹pi³ remis
bool isTie()
{
	return RemainingFields == 0;
}

// Czêœæ algorytmu MinMax wywo³ywana rekurencyjnie
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

//Wywo³anie algorytmu MinMax z wykorzystaniem alpha-beta pruning'u. Ustala najlepszy mo¿liwy ruch dla komputera.
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

// Funkcja wykonuj¹ca ruch komputera i zwracaj¹ca indeks zajmowanego pola
int GetBestComputerMoveFieldIndex() 
{
	Move bestMove = MinMaxAlgorithm();
	GameBoard[bestMove.x][bestMove.y].fieldSymbol = Computer.PlayerSymbol;
	RemainingFields--;
	return GameBoard[bestMove.x][bestMove.y].index;
}

// Funkcja zwracaj¹ca informacjê o tym, czy partiê zaczyna cz³owiek
bool IsHumanStartingGame() 
{
	return HumanStarts;
}

// Funkcja ustalaj¹ca gracza, który rozpocznie nastêpn¹ partiê
bool SetStartingPlayer()
{
	return HumanStarts = !HumanStarts;
}

// Funkcja ustalaj¹ca dane po zwyciêstwie
void SetWinData(Winner winner) 
{
	SetStartingPlayer();
	SomeoneWonTheGame = winner;
}

// Funkcja zwracaj¹ca wyniki gracza i komputera
ScoreTable GetScores() 
{
	return Scores;
}

// Funkcja zwracaj¹ca symbol gracza
Symbol GetHumanSymbol() 
{
	return HumanPlayer.PlayerSymbol;
}

// Funkcja zwracaj¹ca symbol komputera
Symbol GetComputerSymbol()
{
	return Computer.PlayerSymbol;
}

// Funkcja wyonuj¹ca pierwszy ruch komputera (tylko gdy on zaczyna)
int MakeFirstComputerMove() 
{
	int index = rand()%9;
	GetFieldByIndex(index)->fieldSymbol = Computer.PlayerSymbol;
	RemainingFields--;

	return index;
}

// Funkcja wykonuj¹ca ruch gracza na wirtualnej (nie interfejsowej) planszy
void MakePlayerMove(int index) 
{
	GetFieldByIndex(index)->fieldSymbol = HumanPlayer.PlayerSymbol;
	RemainingFields--;
}

// Funkcja dziel¹ca string na podstawie danego regexa
std::vector<std::string> stringSplit(const std::string & string, std::string regexString = ":") 
{
	std::vector<std::string> substrings;

	std::regex regex(regexString);

	std::sregex_token_iterator iter(string.begin(), string.end(), regex, -1);
	std::sregex_token_iterator end;

	while (iter != end) 
	{
		substrings.push_back(*iter);
		++iter;
	}

	return substrings;
}

// Funkcja zapisuj¹ca wynik
void SaveScoresToFile() 
{
	std::fstream plik;
	plik.open("scores.txt", std::ios::out);
	if (plik.good() == true)
	{
		std::string stringToSave = std::to_string(Scores.HumanScore) + ":" + std::to_string(Scores.ComputerScore);
		plik.write(stringToSave.c_str(), sizeof(char)*stringToSave.size());
		plik.close();
	}
}

// Funkcja maj¹ca na celu wczytanie i przypisanie zapisanych wyników
void LoadScoresFromFile()
{
	std::fstream plik;
	plik.open("scores.txt", std::ios::in);
	if (plik.good() == true)
	{
		std::string loadedData;
		getline(plik, loadedData);
		plik.close();

		std::vector<std::string> scoresSubstrings = stringSplit(loadedData);
		Scores.HumanScore = atoi(scoresSubstrings.front().c_str());
		Scores.ComputerScore = atoi(scoresSubstrings.back().c_str());
	}
}

// Funkcja wykonuj¹ca wirtualny ruch gracza i zaraz po tym ruch komputera
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

//Funkcja restartuj¹ca wirtualn¹ grê
void RestartGame() {
	SetGameBoardIndexes();
	SomeoneWonTheGame = Winner::None;
	BestMove = Move();
}