#include "Camera.h"


Camera::Camera() {

	//position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Setup the vector that points upwards.
	//up.x = 0.0f;
	//up.y = 1.0f;
	//up.z = 0.0f;

	//// Setup where the camera is looking by default.
	//lookAt.x = 0.0f;
	//lookAt.y = 0.0f;
	//lookAt.z = 1.0f;

	alignedData = (AlignedData*) _aligned_malloc(sizeof(AlignedData), 16);
	if (alignedData == NULL) {
		MessageBoxA(nullptr, "The data is NULL?!", "Error", MB_OK | MB_ICONERROR);
	}
	alignedData->position = XMVectorZero();
	alignedData->rotation = XMQuaternionIdentity();
}

Camera::~Camera() {
}


//void Camera::render() {
//
//
//}


void Camera::setViewPort(D3D11_VIEWPORT vwprt) {

	viewport = vwprt;
}

D3D11_VIEWPORT Camera::getViewport() {
	return viewport;
}


/* Set the position of the camera in the world. */
//void Camera::setPosition(float positionX, float positionY, float positionZ) {
//
//	position = XMFLOAT3(positionX, positionY, positionZ);
//}

/* Set the position of the camera in the world. */
void XM_CALLCONV Camera::setPosition(FXMVECTOR position) {

	alignedData->position = position;
	viewDirty = true;

}


void XM_CALLCONV Camera::translate(FXMVECTOR translation, Space space) {

	switch (space) {
		case LocalSpace:
			alignedData->position += XMVector3Rotate(translation, alignedData->rotation);
			break;
		case WorldSpace:
			alignedData->position += translation;
			break;
	}

	alignedData->position = XMVectorSetW(alignedData->position, 1.0f);
	viewDirty = true;
	inverseViewDirty = true;

}

XMVECTOR Camera::getPosition() const {
	return alignedData->position;
}


void Camera::setRotation(XMVECTOR rotation) {

	alignedData->rotation = rotation;
}


void Camera::rotate(FXMVECTOR quaternion) {

	alignedData->rotation = XMQuaternionMultiply(alignedData->rotation, quaternion);

	viewDirty = true;
	inverseViewDirty = true;
}

XMVECTOR Camera::getRotation() const {
	return alignedData->rotation;
}





void XM_CALLCONV Camera::setLookAt(FXMVECTOR eye, FXMVECTOR target, FXMVECTOR up) {

	alignedData->viewMatrix = XMMatrixLookAtLH(eye, target, up);

	alignedData->position = eye;
	alignedData->rotation =
		XMQuaternionRotationMatrix(XMMatrixTranspose(alignedData->viewMatrix));

	inverseViewDirty = true;
	viewDirty = false;
}


//XMFLOAT3 Camera::getPosition() {
//	return position;
//}





//void Camera::setViewMatrix(XMMATRIX &view) {
//
//	view = viewMatrix;
//}


XMMATRIX Camera::getViewMatrix() const {

	if (viewDirty)
		updateViewMatrix();
	return alignedData->viewMatrix;
}

XMMATRIX Camera::getInverseViewMatrix() const {

	if (inverseViewDirty) {
		alignedData->inverseViewMatrix = XMMatrixInverse(NULL, alignedData->viewMatrix);
		inverseViewDirty = false;
	}

	return alignedData->inverseViewMatrix;
}

void Camera::setProjection(float fovy, float aspect, float zNear, float zFar) {

	vFoV = fovy;
	aspectRatio = aspect;
	zNear = zNear;
	zFar = zFar;

	projectionDirty = true;
	inverseProjectionDirty = true;
}

XMMATRIX Camera::getInverseProjectionMatrix() const {

	if (inverseProjectionDirty)
		updateInverseProjectionMatrix();

	return alignedData->inverseProjectionMatrix;
}

XMMATRIX Camera::getProjectionMatrix() const {

	if (projectionDirty)
		updateProjectionMatrix();

	return alignedData->projectionMatrix;
}


void Camera::updateViewMatrix() const {

	XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(alignedData->rotation));
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(-(alignedData->position));

	alignedData->viewMatrix = translationMatrix * rotationMatrix;

	inverseViewDirty = true;
	viewDirty = false;
}


void Camera::updateInverseViewMatrix() const {

	if (viewDirty)
		updateViewMatrix();

	alignedData->inverseViewMatrix = XMMatrixInverse(NULL, alignedData->viewMatrix);
	inverseViewDirty = false;
}


void Camera::updateProjectionMatrix() const {

	alignedData->projectionMatrix
		= XMMatrixPerspectiveFovLH(XMConvertToRadians(vFoV), aspectRatio, zNear, zFar);

	projectionDirty = false;
	inverseProjectionDirty = true;
}


void Camera::updateInverseProjectionMatrix() const {

	if (projectionDirty) {
		updateProjectionMatrix();
	}

	alignedData->inverseProjectionMatrix = XMMatrixInverse(NULL, alignedData->projectionMatrix);
	inverseProjectionDirty = false;
}