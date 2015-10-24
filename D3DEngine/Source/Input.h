#pragma once

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>
#include <DirectXMath.h>

using namespace DirectX;

struct Movement {

	float leftRight = 0.0f;
	float backForward = 0.0f;
	float upDown = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};


class Input {
public:
	Input();
	~Input();

	float rotX = 0;
	float rotY = 0;
	float rotZ = 0;
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	void update(double time);

	bool initDirectInput(HINSTANCE hInstance, HWND hwnd);
	void detectInput(double time);
	
	Movement movement;
	//Movement getMovement();

private:
	IDirectInputDevice8* inputKB;
	IDirectInputDevice8* inputMouse;

	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 directInput;

	HWND hwnd;

	

	void release();

};

