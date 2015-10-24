#include <Windows.h>

#include "GraphicsEngine.h"
//#include "Input.h"
#include "Globals.h"

LPCTSTR wndClassName = L"My first Game Engine";
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




void releaseResources() {

	delete gfxEngine;
	//delete input;
}


/** Main windows function.
@nShowWnd how window should be displayed. Examples: SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	gfxEngine = new GraphicsEngine();

	if (!initWindow(hInstance, nShowCmd, Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT, true)) {
		MessageBox(0, TEXT("Window Initialization - Failed"), L"Error", MB_OK);
		releaseResources();
		return 0;

	}

	if (!gfxEngine->initGFXEngine(hInstance, hwnd)) {
		MessageBox(0, L"Direct3D Initialization Failed", L"Error", MB_OK);
		releaseResources();
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
	releaseResources();


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
	wc.style = CS_HREDRAW | CS_VREDRAW;			// mo' styles http://msdn.microsoft.com/en-us/library/ms633574(VS.85).aspx#class_styles
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;					// current app
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// mo' mouse cursors http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// taskbar icon

	if (!RegisterClassEx(&wc)) {

		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(
		NULL,							// extended style, check em out here http://msdn.microsoft.com/en-us/library/61fe4bte(VS.71).aspx
		wndClassName,
		wndClassName,				// title bar text
		WS_OVERLAPPEDWINDOW,			// window style, mo' styles http://msdn.microsoft.com/zh-cn/library/czada357.aspx
		CW_USEDEFAULT, CW_USEDEFAULT,	// top left of window
		Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT,
		NULL,							// handle to parent window
		NULL,							// handle to a menu
		hInstance,						// instance of current program
		NULL);							// used for MDI client window

	if (!hwnd) {

		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, showWnd);
	UpdateWindow(hwnd);


	return true;

}



LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		case WM_CREATE:


			return 0;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
					L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
					DestroyWindow(hwnd);
			}
			return 0;
		case WM_DESTROY:	// top right x button pressed


			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}