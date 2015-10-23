#include "Camera.h"


Camera::Camera() {


}

void Camera::update(Movement move) {

	rotationMatrix = XMMatrixRotationRollPitchYaw(move.pitch, move.yaw, 0);
	XMStoreFloat4(&cameraTarget, XMVector3Normalize(XMVector3TransformCoord(defaultForward, rotationMatrix)));

	XMMATRIX rotateYTempMatrix = XMMatrixRotationY(move.pitch);

	cameraRight = XMVector3TransformCoord(defaultRight, rotateYTempMatrix);
	XMStoreFloat4(&cameraUp, XMVector3TransformCoord(XMLoadFloat4(&cameraUp), rotateYTempMatrix));
	cameraForward = XMVector3TransformCoord(defaultForward, rotateYTempMatrix);

	XMVECTOR tempPos = XMLoadFloat4(&cameraPos);
	tempPos += move.leftRight*cameraRight;
	tempPos += move.backForward*cameraForward;
	XMStoreFloat4(&cameraPos, tempPos);

	move.leftRight = 0.0f;
	move.backForward = 0.0f;

	XMVECTOR tempTarget = XMLoadFloat4(&cameraTarget);
	tempTarget += tempPos;
	XMStoreFloat4(&cameraTarget, tempTarget);

	XMStoreFloat4x4(&cameraView, XMMatrixLookAtLH(tempPos, tempTarget, XMLoadFloat4(&cameraUp)));

}

void Camera::initScene(XMVECTOR position, XMVECTOR target, XMVECTOR up) {


	XMStoreFloat4(&cameraPos, position);
	XMStoreFloat4(&cameraTarget, target);
	XMStoreFloat4(&cameraUp, up);



	XMStoreFloat4x4(&cameraView, XMMatrixLookAtLH(XMLoadFloat4(&cameraPos), XMLoadFloat4(&cameraTarget), XMLoadFloat4(&cameraUp)));
	XMStoreFloat4x4(&cameraProjection, XMMatrixPerspectiveFovLH(0.4f*3.14f,
		(float) Globals::WINDOW_WIDTH / Globals::WINDOW_HEIGHT, 1.0f, cameraDepth));

}


void Camera::setWVP(XMFLOAT4X4 objWorldMatrix) {

	XMStoreFloat4x4(&wvp, XMLoadFloat4x4(&objWorldMatrix) * getView() * getProjection());
}


XMMATRIX Camera::getWVP() {

	return XMLoadFloat4x4(&wvp) * XMLoadFloat4x4(&cameraView) * XMLoadFloat4x4(&cameraProjection);
}


XMMATRIX Camera::getView() {

	return XMLoadFloat4x4(&cameraView);
}


XMMATRIX Camera::getProjection() {

	return XMLoadFloat4x4(&cameraProjection);
}


XMVECTOR Camera::getPosition() {

	return XMLoadFloat4(&cameraPos);
}