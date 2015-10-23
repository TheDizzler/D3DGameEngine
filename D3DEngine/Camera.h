#ifndef _CAMERA_H_
#define _CAMERA_H_

//#define _XM_NO_INTRINSICS_

#include <DirectXMath.h>

#include "globals.h"
#include "Input.h"

using namespace DirectX;


class Camera {
public:

	

	Camera();

	void update(Movement move);

	void initScene(XMVECTOR position, XMVECTOR target, XMVECTOR up);

	void setWVP(XMFLOAT4X4 objWorldMatrix);
	XMMATRIX getWVP();
	XMMATRIX getView();
	XMMATRIX getProjection();
	XMVECTOR getPosition();

private:

	float cameraDepth = 1000.0f;

	XMFLOAT4X4 wvp;
	//XMFLOAT4X4 world;
	XMFLOAT4X4 cameraView;
	XMFLOAT4X4 cameraProjection;

	XMFLOAT4 cameraPos;
	XMFLOAT4 cameraTarget;
	XMFLOAT4 cameraUp;

	XMMATRIX rotationMatrix;

	/*float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
*/
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR cameraForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR cameraRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

};


#endif