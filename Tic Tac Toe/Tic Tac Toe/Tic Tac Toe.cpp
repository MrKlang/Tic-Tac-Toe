// Tic Tac Toe.cpp : Definiuje punkt wejścia dla aplikacji.
//

#include "stdafx.h"
#include "Tic Tac Toe.h"
#include "windowsx.h";
#include "GameControllsHeader.h"
#include <utility>
#include <string>

#define MAX_LOADSTRING 100

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego
const int CellSize = 100;						// Rozmiar komórki
const int LeftOffset = 16;						// Offset z lewej strony, od krawędzi okna w pikselach
const int TopOffset = 16;						// Offset od góry, od krawędzi okna w pikselach
const int RightOffset = 72;						// Offset z prawej strony, od krawędzi okna w pikselach
const COLORREF backgroundColor = RGB(128, 128, 128); // Kolor dla backgroundów dla tekstów w grze
const int CellsInRow = 3;						// Ilość pól w linii
HICON XIcon;									// Handler ikony X
HICON OIcon;									// Handler ikony O

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: W tym miejscu umieść kod.

    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   LoadScoresFromFile();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // Odwołanie podpisujące indeksy "virtualnej" planszy
   SetGameBoardIndexes(); 

   // Zwrot po zamknięciu okienka wyboru symbolu
   const int result = MessageBox(hWnd,
					   L"Do you want to play as X? \n(Otherwise you will play as O)",
					   L"Choose your symbol",
					   MB_ICONINFORMATION | MB_YESNO);

   if (result == IDYES) 
   {
	   SetPlayersSymbols(Symbol::X, Symbol::O);
   }
   else 
   {
	   SetPlayersSymbols(Symbol::O, Symbol::X);
   }

   return TRUE;
}

// Funkcja rysująca background planszy
BOOL GetGameBoardRect(HWND hwnd, RECT * boardRectPointer) 
{
	RECT rect;

	if (GetClientRect(hwnd, &rect))
	{
		boardRectPointer->left = (rect.right - CellSize * CellsInRow) / 2;
		boardRectPointer->right = boardRectPointer->left + CellSize * CellsInRow;

		boardRectPointer->top = (rect.bottom - CellSize * CellsInRow) / 2;
		boardRectPointer->bottom = boardRectPointer->top + CellSize * CellsInRow;

		return TRUE;
	}

	SetRectEmpty(boardRectPointer);
	return FALSE;
}

// Funkcja rysująca siatkę na planszy
void DrawLine(HDC hdc, int x1, int x2, int y1, int y2) 
{
	MoveToEx(hdc,x1,y1,NULL);
	LineTo(hdc,x2,y2);
}

// Funkcja zwracająca index pola 
int GetCellNumber(HWND hwnd, POINT point) 
{
	RECT rect;

	if (GetGameBoardRect(hwnd,&rect)) 
	{
		if (PtInRect(&rect, point)) 
		{
			// Normalizacja
			point = {point.x - rect.left,point.y - rect.top};
			int column = point.x / CellSize;
			int row = point.y / CellSize;

			return column + row * CellsInRow;
		}
	}

	return -1;
}

// Funkcja określająca rozmiar wypełnienia pola symbolu
BOOL GetCellRect(HWND hwnd,int index, RECT * cellRect) 
{
	RECT boardRect;

	SetRectEmpty(cellRect);

	if (GetGameBoardRect(hwnd, &boardRect)) 
	{
		int row = index / CellsInRow;
		int column = index % CellsInRow;

		cellRect->left = boardRect.left + column * CellSize + 1;
		cellRect->right = cellRect->left + CellSize - 1;

		cellRect->top = boardRect.top + row * CellSize + 1;
		cellRect->bottom = cellRect->top + CellSize - 1;

		return TRUE;
	}

	return FALSE;
}

// Funkcja restartująca grę tylko wtedy, gdy gra się zakończyła
void ResetBoardIfGameEnded(HWND hwnd, UINT message, Winner winner)
{
	if (winner != Winner::None)
	{
		int result = MessageBox(hwnd,
					winner == Winner::Tie ? L"It's a tie!\nWant to play again?" :
					winner == Winner::Computer ? L"Computer wins!\nWant to play again?" :
					L"You win!\nWant to play again?",
					L"Game Over",
					MB_ICONINFORMATION | MB_YESNO);
		if (result == IDYES)
		{
			RestartGame();
			RedrawWindow(hwnd, NULL, NULL, message);
		}
		else
		{
			SaveScoresToFile();
			exit(0);
		}
	}
}

// Funkcja wyświetlająca wyniki komputera i gracza
void ShowPlayersScore(HWND hwnd, HDC hdc, RECT windowRect) 
{
	ScoreTable scores = GetScores();
	
	// Złączenie tekstu i wyników dla konkretnych graczy
	std::string humanScoreString = "You: " + std::to_string(scores.HumanScore);
	std::string computerScoreString = std::to_string(scores.ComputerScore)+ " :AI";
	
	// Konwersja formatu, tak aby można go było wyświetlić w oknie gry
	std::wstring humanScoreWString = std::wstring(humanScoreString.begin(), humanScoreString.end());
	std::wstring computerScoreWString = std::wstring(computerScoreString.begin(), computerScoreString.end());

	if (GetClientRect(hwnd, &windowRect)) 
	{
		SetTextColor(hdc, RGB(0, 0, 0));
		SetBkColor(hdc,backgroundColor);

		TextOut(hdc, windowRect.left + LeftOffset, windowRect.top + TopOffset, humanScoreWString.c_str(), humanScoreWString.length());
		TextOut(hdc, windowRect.right - RightOffset, windowRect.top + TopOffset, computerScoreWString.c_str(), computerScoreWString.length());
	}
}

// Funkcja rysująca symbole (ikony) kółka i krzyżyka
void DrawSymbolCentered(HWND hwnd,HDC hdc, int index, Symbol symbolToDraw) 
{
	RECT cellRect;
	const int  IconCenteringOffset = 32; // Offset w pikselach

	if (GetCellRect(hwnd, index, &cellRect))
	{
		DrawIcon(hdc, cellRect.left+IconCenteringOffset, cellRect.top+IconCenteringOffset, symbolToDraw == Symbol::O ? OIcon : XIcon);
	}
}

// Funkcja ograniczająca rozmiar okna gry
void SetWindowSize(LPARAM lParam) {
	MINMAXINFO * pointMinMax = (MINMAXINFO*)lParam;

	pointMinMax->ptMinTrackSize.x = CellSize * 5;
	pointMinMax->ptMinTrackSize.y = pointMinMax->ptMinTrackSize.x;

	pointMinMax->ptMaxTrackSize.x = pointMinMax->ptMinTrackSize.x;
	pointMinMax->ptMaxTrackSize.y = pointMinMax->ptMinTrackSize.x;
}

// Funkcja wyonująca działania po kliknięciu lewym przyciskiem myszy
void HandlePlayerClick(HWND hwnd, HDC hdc, UINT message, int index)
{
	if (index > -1 && index < pow(CellsInRow, 2))
	{
		if (CheckIfMoveIsPossible(index))
		{
			DrawSymbolCentered(hwnd, hdc, index, GetHumanSymbol());

			std::pair<int, Winner> simplePair = UpdateGame(index);

			if (simplePair.first != -1)
			{
				DrawSymbolCentered(hwnd, hdc, simplePair.first, GetComputerSymbol());

				ResetBoardIfGameEnded(hwnd, message, simplePair.second);
			}
			else
			{
				ResetBoardIfGameEnded(hwnd, message, simplePair.second);
			}
		}
	}
}

//Funkcja rysująca planszę w kolejności: biały background, wyniki, linie siatki planszy
void DrawGameBoard(HWND hwnd, HDC hdc) 
{
	RECT windowRect;

	if (GetGameBoardRect(hwnd, &windowRect))
	{
		FillRect(hdc, &windowRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	ShowPlayersScore(hwnd, hdc, windowRect);

	for (int i = 0; i < 4; i++)
	{
		//Pioneowe linie 
		DrawLine(hdc, windowRect.left + CellSize * i, windowRect.left + CellSize * i, windowRect.top, windowRect.bottom);

		//Poziome linie
		DrawLine(hdc, windowRect.left, windowRect.right, windowRect.top + CellSize * i, windowRect.top + CellSize * i);
	}

}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//	WM_LBUTTONDOWN - obsłuż kliknięcie lewym klawiszem myszki
//  WM_GETMINMAXINFO - w tym przypadku ogranicz rozmiar okna
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		XIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_XSYMBOL));
		OIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_OSYMBOL));
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
		{
			POINT clickedPoint{ GET_X_LPARAM(lParam) ,GET_Y_LPARAM(lParam) };
			int index = GetCellNumber(hWnd, clickedPoint);

			HDC hdc = GetDC(hWnd);

			if (hdc != NULL)
			{
				HandlePlayerClick(hWnd, hdc, message, index);
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			
			DrawGameBoard(hWnd, hdc);

			// Jeśli jego kolej to komputer od razu wybierze i narysuje swój symbol
			if (!IsHumanStartingGame())
			{
				int index = MakeFirstComputerMove();
				DrawSymbolCentered(hWnd, hdc, index, GetComputerSymbol());
			}

			EndPaint(hWnd, &ps);
        }
        break;
	case WM_GETMINMAXINFO: 
		{
			SetWindowSize(lParam);
		}
		break;
    case WM_DESTROY:
		DestroyIcon(XIcon);
		DestroyIcon(OIcon);
		SaveScoresToFile();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Procedura obsługi komunikatów dla okna informacji o programie.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
