#include "LightSource.h"

LightSource::LightSource() {
}

LightSource::~LightSource() {
}

void LightSource::setDiffuseColor(float red, float green, float blue, float alpha) {

	diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightSource::setDirection(float x, float y, float z) {

	direction = XMFLOAT3(x, y, z);
}
