#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;


namespace LightSource {


	struct Directional {

		XMFLOAT3 dir;
		float pad1;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		Directional() {
			ZeroMemory(this, sizeof(Directional));
		}
	};


	struct Point {
		XMFLOAT3 dir;
		float pad1;
		XMFLOAT3 pos;
		float range;
		XMFLOAT3 attenuation;
		float pad2;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		Point() {
			ZeroMemory(this, sizeof(Point));
		}
	};
};