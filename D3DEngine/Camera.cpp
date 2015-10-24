#include "Camera.h"


Camera::Camera() {

	//positionX = positionY = positionZ = rotationX = rotationY =rotationZ = 0.0f;
}

Camera::~Camera() {
}

void Camera::setPosition(float x, float y, float z) {

	positionX = x;
	positionY = y;
	positionZ = z;
}

void Camera::setRotation(float x, float y, float z) {

	rotationX = x;
	rotationY = y;
	rotationZ = z;
}

XMFLOAT3 Camera::getPosition() {
	return XMFLOAT3(positionX, positionY, positionZ);
}

XMFLOAT3 Camera::getRotation() {
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void Camera::render() {

	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = positionX;
	position.y = positionY;
	position.z = positionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX * 0.0174532925f;
	yaw = rotationY * 0.0174532925f;
	roll = rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::getViewMatrix(XMMATRIX &view) {

	view = viewMatrix;
}


XMMATRIX Camera::getViewMatrix() {

	return viewMatrix;
}