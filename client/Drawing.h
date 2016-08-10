#pragma once
#include <windows.h>
#include <string>
#include "Tile.h"


// window size in pixels
#define WINDOWSIZE_X 1000
#define WINDOWSIZE_Y 800
// game board size in tiles
#define MAX_X 120
#define MAX_Y 87
// tile size in pixels
#define TILESIZE 8
// menu event IDs
#define ID_FILE_NEWGAME 9001
#define ID_FILE_EXIT 9002


class Drawing
{
	static RECT statusArea; // status text area
	static RECT gameArea; // main game area

	static const HBRUSH clearBrush; // main window color
	static const HBRUSH backgroundBrush; // background color
	static const HBRUSH currentPlayersBrush; // main player's color
	static const HBRUSH otherPlayersBrush; // other player's color

	static const HFONT font; // font for writing status informations - scores, etc.

public:
	static void Init(HWND hWnd);
	static void DrawStatus(HWND hWnd, std::string status);
	static void RedrawWindow(HWND hWnd, Tile board[MAX_X * MAX_Y]);
};
