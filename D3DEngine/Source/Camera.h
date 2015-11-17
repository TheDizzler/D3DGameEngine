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
	void getViewMatrix(XMMATRIX&);
	XMMATRIX getViewMatrix();
private:


	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	XMMATRIX viewMatrix;
	

};
