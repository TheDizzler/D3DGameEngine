#pragma once


/* Global variables and includes */
#include <comdef.h>
#include <string>

namespace Globals {


	static int WINDOW_WIDTH = 1200;
	static int WINDOW_HEIGHT = 800;
	static int SIDEBAR_WIDTH = 400;
	static int SIDEBAR_HEIGHT = WINDOW_HEIGHT;

	static LPCSTR VERTEX_SHADER_VERSION = "vs_5_0";
	static LPCSTR PIXEL_SHADER_VERSION = "ps_5_0";

	static bool vsync_enabled = true;
	static bool FULL_SCREEN = false;

	static float SCREEN_NEAR = 0.1f;
	static float SCREEN_DEPTH = 1000.0f;


	const std::string ASSETS_DIR = "..\\Assets\\";



};
