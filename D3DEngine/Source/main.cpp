#include <CommCtrl.h>

#include "GraphicsEngine.h"
//#include "Input.h"
#include "Globals.h"
#include "../resource.h"


HWND hwnd;
HWND dialog;
HWND adapterComboBox;
//Input* input;

bool closing = false;


double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;

GraphicsEngine* gfxEngine;


int messageLoop();

bool initWindow(HINSTANCE hInstance, int showWnd, bool windowed);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK dialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);



void shutdown() {

	ShowCursor(true);

	if (Globals::FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	delete gfxEngine;
	//delete input;
}


/** Main windows function.
@nShowWnd how window should be displayed. Examples: SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	// Check for DirectX Math library support.
	if (!XMVerifyCPUSupport()) {
		MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return 0;
	}


	if (!initWindow(hInstance, nShowCmd, true)) {
		MessageBox(0, TEXT("Window Initialization - Failed"), L"Error", MB_OK);
		shutdown();
		return 0;

	}


	gfxEngine = new GraphicsEngine();
	if (!gfxEngine->initGFXEngine(hInstance, hwnd, dialog)) {
		MessageBox(0, L"Direct3D Initialization Failed", L"Error", MB_OK);
		shutdown();
		return 0;
	}

	/*if (!gfxEngine->initScene(hwnd)) {
	MessageBox(0, L"Scene Initialization failed", L"Error", MB_OK);
	releaseResources();
	return 0;
	}*/

	/*input = new Input();
	if (!input->initDirectInput(hInstance, hwnd)) {
	MessageBox(0, L"Direct Input Init failed", L"Error", MB_OK);
	releaseResources();
	return 0;
	}*/

	messageLoop(); /* Main program loop */
	shutdown();


	return 0;
}



void startTimer() {

	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;

}


double getTime() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}


double getFrameTime() {

	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}


int messageLoop() {

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // if it's a windows message
			if (msg.message == WM_QUIT)
				break;
			if (dialog == 0 || !IsDialogMessage(dialog, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {	// game code
			frameCount++;
			if (getTime() > 1.0f) {
				fps = frameCount;
				frameCount = 0;
				startTimer();
			}

			double frameTime = getFrameTime();

			//gfxEngine->input->detectInput(frameTime);
			gfxEngine->update(frameTime, fps);
			//gfxEngine->render();
		}

	}

	return msg.wParam;

}


bool initWindow(HINSTANCE hInstance, int showWnd, bool windowed) {

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// taskbar icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// mo' mouse cursors http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = Globals::appName;


	if (!RegisterClassEx(&wc)) {

		MessageBox(NULL, L"Error registering main window class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}



	int posX, posY;
	posX = posY = 0;

	if (Globals::FULL_SCREEN) {

		// Determine the resolution of the clients desktop screen.
		Globals::WINDOW_WIDTH = GetSystemMetrics(SM_CXSCREEN);
		Globals::WINDOW_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long) Globals::WINDOW_WIDTH;
		dmScreenSettings.dmPelsHeight = (unsigned long) Globals::WINDOW_HEIGHT;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.

	} else {
		// If windowed then set it to global default resolution
		// and place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - Globals::WINDOW_WIDTH) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - Globals::WINDOW_HEIGHT) / 2;
	}

	hwnd = CreateWindow(
		Globals::appName,
		L"Main Window",				// title bar text
		WS_OVERLAPPEDWINDOW,
		posX, posY,
		Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT,
		NULL,							// handle to parent window
		NULL,							// handle to a menu
		hInstance,						// instance of current program
		NULL);							// used for MDI client window

	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	UpdateWindow(hwnd);

	ShowCursor(true);


	adapterComboBox = GetDlgItem(dialog, IDC_COMBO_ADAPTERS);



	return true;

}


LRESULT CALLBACK messageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
					L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
					DestroyWindow(hwnd);
			}
			return 0;


		case WM_KEYUP:
			// If a key is released then send it to the input object so it can unset the state for that key.
			//m_Input->KeyUp((unsigned int) wParam);
			return 0;


			// Any other messages send to the default message handler as our application won't make use of them.
		default:
			return DefWindowProc(hwnd, umsg, wParam, lParam);
	}
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

		case WM_CREATE:

			dialog = CreateDialog(((LPCREATESTRUCT) lParam)->hInstance,
				MAKEINTRESOURCE(DIALOG_CONFIG), hwnd, (DLGPROC) dialogProc);
			ShowWindow(dialog, SW_SHOW);
			RECT rcMain, rcDialog;
			GetWindowRect(hwnd, &rcMain);
			SetWindowPos(dialog, 0, rcMain.left, rcMain.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			break;
		case WM_DESTROY:	// top right x button pressed
			closing = true;
			SendMessage(dialog, WM_DESTROY, 0, 0);
			PostQuitMessage(0);
			return 0;
			// Check if the window is being closed.
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		default:
			return messageHandler(hwnd, msg, wParam, lParam);

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


INT_PTR CALLBACK dialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_COMBO_ADAPTERS: // If the combo box sent the message,
					switch (HIWORD(wParam)) // Find out what message it was
					{
						case CBN_DROPDOWN: // This means that the list is about to display
							break;
						case CBN_CLOSEUP:
							break;
						case CBN_SELENDOK:
							if (!gfxEngine->changeAdapters(hwnd)) {
								ErrorMessage("Bad news");
								PostQuitMessage(0);
							}
							break;
					}
				case WM_CLOSE:
					return INT_PTR(FALSE);
				case WM_DESTROY:
					if (closing)
						return EndDialog(hwndDlg, SW_HIDE);
					return INT_PTR(FALSE);
			}
		default:
			return INT_PTR(FALSE);
	}

}