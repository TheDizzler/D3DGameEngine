#pragma once


#include "D3D.h"
#include "TextFactory.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"
#include "TextureShader.h"


class GraphicsEngine : public D3D {
public:

	TextFactory *textFactory;
	TextLabel *timer;

	GraphicsEngine();
	~GraphicsEngine();

	bool initGFXEngine(HINSTANCE hInstance, HWND hwnd);
	void shutdown();

	void render();
	//void beginDraw();
	//void endDraw();
	void update(double time, int fps);



private:

	Camera* camera;
	Model* model;

	ColorShader* colorShader;
	TextureShader* textureShader;

	float bgColor[4];

	void testText();



};