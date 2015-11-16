#include "DiffuseLight.h"



DiffuseLight::DiffuseLight() {
}


DiffuseLight::~DiffuseLight() {
}



void DiffuseLight::setDiffuseColor(float red, float green, float blue, float alpha) {

	diffuseColor = XMFLOAT4(red, green, blue, alpha);
}



void DiffuseLight::setAmbientColor(float red, float green, float blue, float alpha) {

	ambientColor = XMFLOAT4(red, green, blue, alpha);
}
