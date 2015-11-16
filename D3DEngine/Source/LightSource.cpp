#include "LightSource.h"

LightSource::LightSource() {
}

LightSource::~LightSource() {
}


void LightSource::setDirection(float x, float y, float z) {

	direction = XMFLOAT3(x, y, z);
}
