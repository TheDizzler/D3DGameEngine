#pragma once


/* Global variables and includes */


namespace Globals {

	static LPCTSTR appName = L"My Second Game Engine";
	static LPCTSTR configWindowName = L"Config Window";
	static int WINDOW_WIDTH = 1440;
	static int WINDOW_HEIGHT = 900;
	static int SIDEBAR_WIDTH = 400;
	static int SIDEBAR_HEIGHT = WINDOW_HEIGHT;

	static LPCSTR VERTEX_SHADER_VERSION = "vs_5_0";
	static LPCSTR PIXEL_SHADER_VERSION = "ps_5_0";

	/* 0 = Present as fast as possible.
		1 = Lock to screen refresh rate. */
	static int vsync_enabled = 1;
	static bool FULL_SCREEN = false;

	static float SCREEN_NEAR = 0.1f;
	static float SCREEN_DEPTH = 1000.0f;

	
	static const WCHAR* ASSETS_DIR = L"../../D3DEngine/Assets/";



};
