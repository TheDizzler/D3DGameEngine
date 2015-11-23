#include "Camera.h"


Camera::Camera() {

	position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
}

Camera::~Camera() {
}


/* Set the position of the camera in the world. */
void Camera::setPosition(float positionX, float positionY, float positionZ) {

	position = XMFLOAT3(positionX, positionY, positionZ);
}


void Camera::setRotation(float x, float y, float z) {

	rotation = XMFLOAT3(x, y, z);
}


void Camera::setLookAt(float positionX, float positionY, float positionZ) {

	lookAt = XMFLOAT3(positionX, positionY, positionZ);
}

XMFLOAT3 Camera::getPosition() {
	return position;
}

XMFLOAT3 Camera::getRotation() {
	return rotation;
}

void Camera::render() {

	
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);


	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotation.x * 0.0174532925f;
	yaw = rotation.y * 0.0174532925f;
	roll = rotation.z * 0.0174532925f;

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


void Camera::setViewMatrix(XMMATRIX &view) {

	view = viewMatrix;
}


XMMATRIX Camera::getViewMatrix() {

	return viewMatrix;
}


