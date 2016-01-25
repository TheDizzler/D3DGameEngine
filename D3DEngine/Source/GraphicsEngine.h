#pragma once


#include "D3D.h"
#include "ShaderManager.h"
#include "TextFactory.h"
#include "Camera.h"
//#include "Model.h"
#include "BasicModel.h"

/** Container for all graphical properties. For now it is the whole engine, including gameplay
	elements but this will eventually be changed. */
class GraphicsEngine : public D3D {
public:


	ID3D11Buffer* constantBuffers[NumConstantBuffers];


	TextFactory *textFactory = 0;
	TextLabel *timer = 0;

	GraphicsEngine();
	~GraphicsEngine();

	bool initGFXEngine(HINSTANCE hInstance, HWND hwnd);
	void shutdown();

	void render();
	//void beginDraw();
	//void endDraw();
	void update(double time, int fps);
	
	void initializeViewPort();

private:

	Camera camera;
	DiffuseLight* light = 0;
	Model* model = 0;
	BasicModel* basicModel = 0;

	MeshLoader* meshLoader = 0;

	ShaderManager* shaderManager = 0;

	float clearDepth = 1.0f;
	float clearStencil = 0.0f;

	D3D11_VIEWPORT viewport = {0};

	/* Used to transform objects vertices from view space to clip space. */
	XMMATRIX projectionMatrix;
	//XMMATRIX worldMatrix; // this is a per object matrix
	XMMATRIX viewMatrix;
	//XMMATRIX orthoMatrix;

	float clearColor[4];

	bool createConstantBuffer();

	void testText();

};