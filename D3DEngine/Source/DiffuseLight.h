#pragma once

#include "LightSource.h"

class DiffuseLight : public LightSource {
public:

	XMFLOAT4 diffuseColor;
	XMFLOAT4 ambientColor;


	DiffuseLight();
	~DiffuseLight();


	void setDiffuseColor(float red, float green, float blue, float alpha);
	void setAmbientColor(float red, float green, float blue, float alpha);
};

