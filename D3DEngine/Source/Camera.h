#pragma once

#include <DirectXMath.h>
//#include "globals.h"

using namespace DirectX;


class Camera {
public:

	Camera();
	~Camera();

	void setPosition(float, float, float);
	void setRotation(float, float, float);

	XMFLOAT3 getPosition();
	XMFLOAT3 getRotation();

	void render();
	void getViewMatrix(XMMATRIX&);
	XMMATRIX getViewMatrix();
private:
	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	XMMATRIX viewMatrix;
	

};
