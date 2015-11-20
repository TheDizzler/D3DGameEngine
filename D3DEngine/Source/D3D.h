#pragma once


#include "Globals.h"
#include "Utils.h"

using namespace DirectX;
using namespace std;


class D3D {
public:

	D3D();
	~D3D();

	bool initD3D(HINSTANCE hInstance, HWND hwnd);

protected:



	/* Changes backbuffer to front buffer. */
	IDXGISwapChain* swapChain = 0;
	/* GPU object */
	ID3D11Device* device = 0;
	/* GPU interface */
	ID3D11DeviceContext* deviceContext = 0;
	/* The backbuffer that gets drawn to. */
	ID3D11RenderTargetView* renderTargetView = 0;
	/* Depth Buffer */
	ID3D11Texture2D* depthStencilBuffer = 0;
	/* Defines functionality of depth/stencil stages. */
	ID3D11DepthStencilState* depthStencilState = 0;
	/* Defines functionality of rasterizer stage. */
	ID3D11RasterizerState* rasterState = 0;
	ID3D11DepthStencilView* depthStencilView = 0;


	D3D_FEATURE_LEVEL featureLevel;
	vector<IDXGIAdapter*> adapters;
	vector<IDXGIOutput*> adapterOutputs;
	unsigned int numModes;
	DXGI_MODE_DESC* displayModeList; // all possible display modes with this monitor/video card combination.
	int videoCardMemory;
	char videoCardDescription[128];


	D3D11_INPUT_ELEMENT_DESC layout[4];
	UINT numLayoutElements;

	bool initializeSwapChain(HWND hwnd);
	bool initializeDepthStencil();
	bool initializeRasterizer();


	bool getDisplayAdapters(UINT* numerator, UINT* denominator);
	void getVideoCardInfo(char* cardName, int& memory);

	void getLatestShaderVersions();
	string getLatestVSVersion();
	string getLatestPSVersion();

	void shutdownD3D();
};

