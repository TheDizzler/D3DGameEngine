#pragma once



using namespace DirectX;


class Camera {
public:

	// When performing transformations on the camera, 
	// it is sometimes useful to express which space this 
	// transformation should be applied.
	enum Space {
		LocalSpace,
		WorldSpace,
	};


	Camera();
	~Camera();

	//void render();

	void setViewPort(D3D11_VIEWPORT viewport);
	D3D11_VIEWPORT getViewport();

	//void setPosition(float x, float y, float z);
	void XM_CALLCONV setPosition(FXMVECTOR position);
	void XM_CALLCONV translate(FXMVECTOR translation, Space space);
	XMVECTOR getPosition() const;

	void setRotation(FXMVECTOR rotation);
	void rotate(FXMVECTOR quaternion);
	XMVECTOR getRotation() const;


	//void setLookAt(float lookAtX, float lookAtY, float lookAtZ);
	void XM_CALLCONV setLookAt(FXMVECTOR eye, FXMVECTOR target, FXMVECTOR up);
	XMMATRIX getViewMatrix() const;
	//void setViewMatrix(XMMATRIX&);
	
	XMMATRIX getInverseViewMatrix() const;

	/** Set the camera to a perspective projection matrix.
	* @param fovy The vertical field of view in degrees.
	* @param aspect The aspect ratio of the screen.
	* @param zNear The distance to the near clipping plane.
	* @param zFar The distance to the far clipping plane.*/
	void setProjection(float fovy, float aspect, float zNear, float zFar);
	XMMATRIX getProjectionMatrix() const;
	XMMATRIX getInverseProjectionMatrix() const;


	void updateViewMatrix() const;
	void updateInverseViewMatrix() const;
	void updateProjectionMatrix() const;
	void updateInverseProjectionMatrix() const;

private:

	/*XMMATRIX viewMatrix;
	XMFLOAT3 up, position, lookAt, rotation;*/
	D3D11_VIEWPORT viewport;


	// This data must be aligned otherwise the SSE intrinsics fail
	// and throw exceptions.
	__declspec(align(16)) struct AlignedData {
		/* World-space position of the camera. */
		XMVECTOR position;
		/* World-space rotation of the camera.
			THIS IS A QUATERNION!!!! */
		XMVECTOR rotation;

		XMMATRIX viewMatrix, inverseViewMatrix;
		XMMATRIX projectionMatrix, inverseProjectionMatrix;
	};
	AlignedData* alignedData;

	// projection parameters
	float vFoV;   // Vertical field of view.
	float aspectRatio; // Aspect ratio
	float zNear;      // Near clip distance
	float zFar;       // Far clip distance.

	// True if the view matrix needs to be updated.
	mutable bool viewDirty, inverseViewDirty;
	// True if the projection matrix needs to be updated.
	mutable bool projectionDirty, inverseProjectionDirty;
};
