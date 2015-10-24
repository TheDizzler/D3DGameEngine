#include "Input.h"


Input::Input() {}


Input::~Input() {

	release();
}


bool Input::initDirectInput(HINSTANCE hInstance, HWND hw) {

	hwnd = hw;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**) &directInput, NULL)))

		return false;

	if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &inputKB, NULL)))

		return false;

	if (FAILED(directInput->CreateDevice(GUID_SysMouse, &inputMouse, NULL)))

		return false;

	if (FAILED(inputKB->SetDataFormat(&c_dfDIKeyboard)))

		return false;

	if (FAILED(inputKB->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))

		return false;

	if (FAILED(inputMouse->SetDataFormat(&c_dfDIMouse)))

		return false;

	if (FAILED(inputMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND)))

		return false;

	ZeroMemory(&movement, sizeof(Movement));

	return true;
}


void Input::detectInput(double time) {

	DIMOUSESTATE mouseCurrentState;

	BYTE keyboardState[256];

	inputKB->Acquire();
	inputMouse->Acquire();

	inputMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
	inputKB->GetDeviceState(sizeof(keyboardState), (LPVOID) &keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80) {
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		return;
	}


	float speed = 15.0f * time;

	if (keyboardState[DIK_A] & 0x80)
		movement.leftRight -= speed;

	if (keyboardState[DIK_D] & 0x80)
		movement.leftRight += speed;

	if (keyboardState[DIK_W] & 0x80)
		movement.backForward += speed;

	if (keyboardState[DIK_S] & 0x80)
		movement.backForward -= speed;

	if (mouseCurrentState.lX != mouseLastState.lX || mouseCurrentState.lY != mouseLastState.lY) {
		movement.yaw += mouseLastState.lX * 0.001f;
		movement.pitch += mouseCurrentState.lY * 0.001f;
		mouseLastState = mouseCurrentState;
	}

}

//Movement Input::getMovement() {
//
//	return movement;
//}


void Input::release() {

	inputKB->Unacquire();
	inputMouse->Unacquire();
	directInput->Release();
}



