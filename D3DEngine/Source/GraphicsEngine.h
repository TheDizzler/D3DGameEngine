#pragma once


#include "D3D.h"
#include "TextFactory.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "LightSource.h"

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
	LightSource* light = 0;
	Model* model = 0;
	Mesh* mesh = 0;

	ColorShader* colorShader = 0;
	TextureShader* textureShader = 0;
	LightShader* lightShader = 0;

	float bgColor[4];

	void testText();


	const WCHAR* COLOR_VERTEX_SHADER = L"./source/shaders/VertexShader.hlsl";
	const WCHAR* COLOR_PIXEL_SHADER = L"./source/shaders/PixelShader.hlsl";
	const WCHAR* TEXTURE_VERTEX_SHADER = L"./source/shaders/TextureVertexShader.hlsl";
	const WCHAR* TEXTURE_PIXEL_SHADER = L"./source/shaders/TexturePixelShader.hlsl";
	const WCHAR* LIGHT_VERTEX_SHADER = L"./source/shaders/LightVertexShader.hlsl";
	const WCHAR* LIGHT_PIXEL_SHADER = L"./source/shaders/LightPixelShader.hlsl";
};