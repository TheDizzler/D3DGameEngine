#pragma once



using namespace DirectX;


class Camera {
public:

	Camera();
	~Camera();

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);

	XMFLOAT3 getPosition();
	XMFLOAT3 getRotation();

	void render();
	void setViewMatrix(XMMATRIX&);
	void setLookAt(float lookAtX, float lookAtY, float lookAtZ);

	XMMATRIX getViewMatrix();

private:

	XMMATRIX viewMatrix;
	XMFLOAT3 up, position, lookAt, rotation;

};
