#pragma once


#include "D3D.h"
#include "TextFactory.h"
#include "Camera.h"
#include "Model.h"
#include "ShaderManager.h"


/** Container for all graphical properties. For now it is the whole engine, including gameplay
	elements but this will eventually be changed. */
class GraphicsEngine : public D3D {
public:

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
	


private:

	Camera* camera = 0;
	DiffuseLight* light = 0;
	Model* model = 0;
	Mesh* mesh = 0;

	ShaderManager* shaders = 0;

	float bgColor[4];

	void testText();

};