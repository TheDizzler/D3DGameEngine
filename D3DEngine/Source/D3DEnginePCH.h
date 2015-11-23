#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <dinput.h>
#include <comdef.h>
#include <stdio.h>
#include <wchar.h>

#include <fstream>
#include <vector>
#include <sstream>

#include "..\..\FW1FontWrapper\FW1FontWrapper.h"

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "FW1FontWrapper.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

enum TRESULTS {
	NO_WORRIES,
	PROBLEMS,
	NO_SHADER_ERROR,
};

/* Constant Buffers store shader variables that remain constant during a draw call. */
enum ConstantBuffer {
	ApplicationBuffer,	// variables that rarely change, ex: camera projection matrix
	PerFrameBuffer,		// variables that change each frame ex: camera view matrix
	PerObjectBuffer,	// variables that are different for every object rendered
	NumConstantBuffers
};

inline void ErrorMessage(std::string msg) {

	std::wstring wstr;
	for (int i = 0; i < msg.size(); ++i)
		wstr += wchar_t(msg[i]);
		MessageBox(NULL, wstr.c_str(), L"ERROR", MB_OK);
	
}


inline void QuickMessage(wchar_t* msg) {

	MessageBox(NULL, msg, L"This is an Quick Msg", MB_OK);
}

// Safely release a COM object.
template<typename T>
inline void safeRelease(T& ptr) {
	if (ptr != NULL) {
		ptr->Release();
		ptr = NULL;
	}
}
