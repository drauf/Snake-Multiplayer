#include "Drawing.h"


RECT Drawing::statusArea = { 10, 5, 200, 30 }; // status text area
RECT Drawing::gameArea = { 10, 37, 971, 726 }; // main game area

const HBRUSH Drawing::currentPlayersBrush = CreateSolidBrush(RGB(80, 180, 80)); // green
const HBRUSH Drawing::otherPlayersBrush = CreateSolidBrush(RGB(50, 40, 20)); // black
const HBRUSH Drawing::backgroundBrush = CreateSolidBrush(RGB(200, 230, 200)); // very light green
const HBRUSH Drawing::clearBrush = CreateSolidBrush(RGB(0, 100, 200)); // blue

const HFONT Drawing::font = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
	OUT_TT_PRECIS, 0, CLEARTYPE_QUALITY, FF_MODERN, "Courier New");


void Drawing::Init(HWND hWnd)
{
	HMENU hMenu, hSubMenu;

	hMenu = CreateMenu();

	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_NEWGAME, "&New Game");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "&Exit");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, UINT(hSubMenu), "&File");

	SetMenu(hWnd, hMenu);
}


void Drawing::DrawStatus(HWND hWnd, std::string status)
{
	PAINTSTRUCT PaintStruct;
	HDC hdc = BeginPaint(hWnd, &PaintStruct);

	// draw with background color over the old status
	FillRect(hdc, &statusArea, backgroundBrush);

	// draw the new status
	SelectObject(hdc, font);
	SetBkColor(hdc, RGB(0, 0, 0)); // black
	SetTextColor(hdc, RGB(255, 255, 255)); // white
	DrawText(hdc, status.c_str(), -1, &statusArea, DT_LEFT);

	EndPaint(hWnd, &PaintStruct);
}


void Drawing::RedrawWindow(HWND hWnd, Tile board[MAX_X * MAX_Y])
{
	PAINTSTRUCT PaintStruct;
	HDC hdc = BeginPaint(hWnd, &PaintStruct);

	// setup offscreen DC
	auto hdcBuffer = CreateCompatibleDC(hdc);
	auto hBitmapBuffer = CreateCompatibleBitmap(hdc, WINDOWSIZE_X, WINDOWSIZE_Y);
	SelectObject(hdcBuffer, hBitmapBuffer);

	// draw to the offscreen DC
	static RECT clientArea;
	GetClientRect(hWnd, &clientArea);
	FillRect(hdcBuffer, &clientArea, clearBrush);
	FillRect(hdcBuffer, &gameArea, backgroundBrush);

	static RECT drawPos;
	for (auto index = 0; index < MAX_X * MAX_Y; index++)
	{
		if (board[index].type == EMPTY) break;

		auto x = board[index].x;
		auto y = board[index].y;

		drawPos.top = (y + 2)*TILESIZE + 22;
		drawPos.bottom = drawPos.top + TILESIZE - 1;
		drawPos.left = x*TILESIZE + 11;
		drawPos.right = drawPos.left + TILESIZE - 1;

		FillRect(hdcBuffer, &drawPos, (board[index].type == CURRENT_PLAYER) ? currentPlayersBrush : otherPlayersBrush);
	}

	// copy the content of offscreen DC to actual screen DC and free memory
	BitBlt(hdc, 0, 0, WINDOWSIZE_X, WINDOWSIZE_Y, hdcBuffer, 0, 0, SRCCOPY);
	DeleteDC(hdcBuffer);
	DeleteObject(hBitmapBuffer);
	EndPaint(hWnd, &PaintStruct);
}
