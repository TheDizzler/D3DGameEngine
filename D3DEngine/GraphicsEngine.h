#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Globals.h"
#include "TextFactory.h"

using namespace DirectX;

class GraphicsEngine {
public:

	TextFactory *textFactory;
	TextLabel *timer;

	GraphicsEngine();
	~GraphicsEngine();

	bool initD3D(HINSTANCE hInstance, HWND hwnd);
	void shutdown();
	
	void draw();
	//void beginDraw();
	//void endDraw();
	void update(double time, int fps);

	void getVideoCardInfo(char* cardName, int& memory);

private:

	bool m_vsync_enabled = false;
	bool fullscreen = false;

	float screenNear = 0;
	float screenDepth = 1;

	D3D11_INPUT_ELEMENT_DESC layout[4];
	UINT numLayoutElements;

	float bgColor[4];

	vector <IDXGIAdapter*> adapters;
	DXGI_MODE_DESC* displayModeList; // all possible display modes with this monitor/video card combination.
	int videoCardMemory;
	char videoCardDescription[128];

	/* Changes backbuffer to front buffer. */
	IDXGISwapChain* swapChain;
	/* GPU object */
	ID3D11Device* device;
	/* GPU interface */
	ID3D11DeviceContext* deviceContext;
	/* The backbuffer that gets drawn to. */
	ID3D11RenderTargetView* renderTargetView;

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;


	bool getDisplayAdapters(UINT* numerator, UINT* denominator);

};