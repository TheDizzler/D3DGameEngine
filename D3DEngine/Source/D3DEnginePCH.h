#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <dinput.h>
#include <comdef.h>
#include <stdio.h>

#include <fstream>
#include <vector>
#include <sstream>

#include "..\..\FW1FontWrapper\FW1FontWrapper.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment (lib, "FW1FontWrapper.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")



// Safely release a COM object.
template<typename T>
inline void safeRelease(T& ptr) {
	if (ptr != NULL) {
		ptr->Release();
		ptr = NULL;
	}
}