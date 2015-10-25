#pragma once


#include "D3D.h"
#include "TextFactory.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "LightSource.h"


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
	LightSource* light;
	Model* model;

	ColorShader* colorShader;
	TextureShader* textureShader;
	LightShader* lightShader;

	float bgColor[4];

	void testText();


	const WCHAR* COLOR_VERTEX_SHADER = L"./source/shaders/VertexShader.hlsl";
	const WCHAR* COLOR_PIXEL_SHADER = L"./source/shaders/PixelShader.hlsl";
	const WCHAR* TEXTURE_VERTEX_SHADER = L"./source/shaders/TextureVertexShader.hlsl";
	const WCHAR* TEXTURE_PIXEL_SHADER = L"./source/shaders/TexturePixelShader.hlsl";
	const WCHAR* LIGHT_VERTEX_SHADER = L"./source/shaders/LightVertexShader.hlsl";
	const WCHAR* LIGHT_PIXEL_SHADER = L"./source/shaders/LightPixelShader.hlsl";
};