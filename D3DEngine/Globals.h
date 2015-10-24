#pragma once


/* Global variables and includes */
#include <comdef.h>

namespace Globals {

	const int WINDOW_WIDTH = 1200;
	const int WINDOW_HEIGHT = 800;
	const int SIDEBAR_WIDTH = 400;
	const int SIDEBAR_HEIGHT = WINDOW_HEIGHT;

	const LPCSTR VERTEX_SHADER_VERSION = "vs_5_0";
	const LPCSTR PIXEL_SHADER_VERSION = "ps_5_0";

	const bool vsync_enabled = false;
	const bool fullscreen = false;

	const float SCREEN_NEAR = 0.0f;
	const float SCREEN_DEPTH = 1000.0f;

};
