#include "ClientGame.h"
#include "Drawing.h"
#include "DirectionEnum.h"

#define BUFFER_SIZE 100

const int arraySize = 20;
char serverIP[arraySize], serverPort[arraySize];
HWND mainWindowHandler;

LRESULT CALLBACK WndProc2(HWND secondWindowHandler, UINT message, WPARAM wParam, LPARAM lParam);

bool isWindow2Open = true;
bool isWindow2Registered = false;

void createWindow2(WNDCLASSW& wc2, HWND& hwnd, HINSTANCE hInst, int nShowCmd);

ClientGame *client;
TileTypeEnum board[MAX_X][MAX_Y];


void RestartGame(HWND hWnd)
{
	Drawing::RedrawWindow(hWnd, board);
	// clear the board array
	memset(board, 0, sizeof(board[0][0]) * MAX_X * MAX_Y);

	char *ip = serverIP;
	char *port = serverPort;
	client = new ClientGame(ip, port, hWnd);
}


void HandleMenuSelection(HWND hWnd, WPARAM param)
{
	switch (LOWORD(param)) {
	case ID_FILE_NEWGAME:
		isWindow2Open = false;
		break;
	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;
	}
}


void HandleKeyboardInput(HWND hWnd, WPARAM input)
{
	switch (input) {
	case VK_RIGHT:
		client->sendActionPacket(RIGHT);
		break;
	case VK_LEFT:
		client->sendActionPacket(LEFT);
		break;
	case VK_UP:
		client->sendActionPacket(UP);
		break;
	case VK_DOWN:
		client->sendActionPacket(DOWN);
		break;
	case VK_SPACE:
		client->sendReadyPacket();
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN: // handle keyboard input
		HandleKeyboardInput(hWnd, wParam);
		break;
	case WM_PAINT: // redraw window
		Drawing::RedrawWindow(hWnd, board);
		break;
	case WM_COMMAND: // handle menu selection 
		HandleMenuSelection(hWnd, wParam);
		break;
	case WM_CREATE: // create window
		Drawing::Init(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd)
{
	WNDCLASSW secondWindowClass; 
	HWND secondWindowHandler;  

	MSG msg;
	client = nullptr;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, WndProc, 0, 0, hInstance, nullptr,
		nullptr, HBRUSH(COLOR_WINDOW + 1), nullptr, "wndClass", nullptr };

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// create the window
	HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "wndClass", "Multiplayer Snake!", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOWSIZE_X, WINDOWSIZE_Y, nullptr, nullptr, hInstance, nullptr);
	mainWindowHandler = hWnd;
	if (hWnd == nullptr)
	{
		MessageBox(nullptr, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	// enter the message loop - listen for events and then send them to WndProc()
	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		if (client != nullptr) client->update(board);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (isWindow2Open == false) {
			createWindow2(secondWindowClass, secondWindowHandler, hInstance, nShowCmd);
			isWindow2Open = true;
		}	
	}

	UnregisterClass("wndClass", wc.hInstance);
	return 0;
}

void createWindow2(WNDCLASSW& wc, HWND& hWnd, HINSTANCE hInst, int nShowCmd) {
	if (isWindow2Registered == false) {
		ZeroMemory(&wc, sizeof(WNDCLASSW));
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.hInstance = hInst;
		wc.lpfnWndProc = (WNDPROC)WndProc2;
		wc.lpszClassName = L"wc2";
		wc.lpszMenuName = NULL;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClassW(&wc))
		{
			int nResult = GetLastError();
			MessageBox(NULL,
				"Window class creation failed",
				"Window Class Failed",
				MB_ICONERROR);
		}
		else
			isWindow2Registered = true;
	}
	HWND hwnd = CreateWindowW(wc.lpszClassName, L"Network settings",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		220, 220, 280, 200, 0, 0, hInst, 0);


	if (!hwnd)
	{
		int nResult = GetLastError();

		MessageBox(NULL,
			"Window creation failed",
			"Window Creation Failed",
			MB_ICONERROR);
	}

	ShowWindow(hwnd, nShowCmd);
}

LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndIP, ipLabel;
	static HWND hwndPort, portLabel;
	HWND hwndButton;

	switch (msg) {

	case WM_CREATE:
		ipLabel = CreateWindowW(L"static", L"IP:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			30, 20, 30, 20, hwnd, (HMENU)1,
			NULL, NULL);

		portLabel = CreateWindowW(L"static", L"Port:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			20, 50, 30, 20, hwnd, (HMENU)1,
			NULL, NULL);

		hwndIP = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 20, 150, 20, hwnd, (HMENU)2,
			NULL, NULL);

		hwndPort = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 50, 150, 20, hwnd, (HMENU)2,
			NULL, NULL);

		hwndButton = CreateWindowW(L"button", L"Connect",
			WS_VISIBLE | WS_CHILD, 90, 100, 80, 25,
			hwnd, (HMENU)3, NULL, NULL);

		break;

	case WM_COMMAND:

		if (HIWORD(wParam) == BN_CLICKED) {
			wchar_t ip[arraySize], port[arraySize];
			size_t i;
			GetWindowTextW(hwndIP, ip, arraySize);
			GetWindowTextW(hwndPort, port, arraySize);
			memset(serverIP, 0, 20 + 1);
			wcstombs_s(&i, serverIP, (size_t)BUFFER_SIZE, ip, (size_t)BUFFER_SIZE);
			memset(serverPort, 0, 20 + 1);
			wcstombs_s(&i, serverPort, (size_t)BUFFER_SIZE, port, (size_t)BUFFER_SIZE);
			RestartGame(mainWindowHandler);
			DestroyWindow(hwnd);
		}
		break;

	case WM_DESTROY:
		isWindow2Open = true;
		DestroyWindow(hwnd);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}