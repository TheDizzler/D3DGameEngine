#pragma once

#include <DirectXMath.h>

using namespace DirectX;


class LightSource {
public:

	LightSource();
	~LightSource();


	void setDiffuseColor(float red, float green, float blue, float alpha);
	void setDirection(float x, float y, float z);


	XMFLOAT3 direction;
	XMFLOAT4 diffuseColor;

	
};


//struct Directional {
	//
	//	XMFLOAT3 dir;
	//	float pad1;
	//	XMFLOAT4 ambient;
	//	XMFLOAT4 diffuse;
	//
	//	Directional() {
	//		ZeroMemory(this, sizeof(Directional));
	//	}
	//};
	//
	//
	//struct Point {
	//	XMFLOAT3 dir;
	//	float pad1;
	//	XMFLOAT3 pos;
	//	float range;
	//	XMFLOAT3 attenuation;
	//	float pad2;
	//	XMFLOAT4 ambient;
	//	XMFLOAT4 diffuse;
	//
	//	Point() {
	//		ZeroMemory(this, sizeof(Point));
	//	}
	//};