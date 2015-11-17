#include "GraphicsEngine.h"
//#include "Input.h"
#include "Globals.h"


HWND hwnd;
//Input* input;

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;

GraphicsEngine* gfxEngine;


int messageLoop();

bool initWindow(HINSTANCE hInstance, int showWnd, int width, int height, bool windowed);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




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

	gfxEngine = new GraphicsEngine();

	if (!initWindow(hInstance, nShowCmd, Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT, true)) {
		MessageBox(0, TEXT("Window Initialization - Failed"), L"Error", MB_OK);
		shutdown();
		return 0;

	}

	if (!gfxEngine->initGFXEngine(hInstance, hwnd)) {
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
			TranslateMessage(&msg);
			DispatchMessage(&msg);
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
			gfxEngine->render();
		}

	}

	return msg.wParam;

}


bool initWindow(HINSTANCE hInstance, int showWnd, int width, int height, bool windowed) {

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// mo' mouse cursors http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = Globals::appName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// taskbar icon

	if (!RegisterClassEx(&wc)) {

		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Determine the resolution of the clients desktop screen.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;
	int posX, posY;

	if (Globals::FULL_SCREEN) {
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long) screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long) screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	} else {
		// If windowed then set it to 800x600 resolution.
		screenWidth = Globals::WINDOW_WIDTH;
		screenHeight = Globals::WINDOW_HEIGHT;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		Globals::appName,
		Globals::appName,				// title bar text
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY,
		screenWidth, screenHeight,
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

		case WM_DESTROY:	// top right x button pressed

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