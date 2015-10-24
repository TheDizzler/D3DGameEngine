#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Globals.h"


using namespace DirectX;
using namespace std;


class D3D {
public:
	D3D();
	~D3D();

	bool initD3D(HINSTANCE hInstance, HWND hwnd);

protected:



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


	vector<IDXGIAdapter*> adapters;
	vector<IDXGIOutput*> adapterOutputs;
	unsigned int numModes;
	DXGI_MODE_DESC* displayModeList; // all possible display modes with this monitor/video card combination.
	int videoCardMemory;
	char videoCardDescription[128];


	D3D11_INPUT_ELEMENT_DESC layout[4];
	UINT numLayoutElements;

	bool getDisplayAdapters(UINT* numerator, UINT* denominator);
	void getVideoCardInfo(char* cardName, int& memory);

	void shutdownD3D();
};

