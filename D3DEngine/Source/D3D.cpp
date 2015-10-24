#include "D3D.h"


D3D::D3D() {
}


D3D::~D3D() {
}

bool D3D::initD3D(HINSTANCE hInstance, HWND hwnd) {



	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;

	unsigned int numerator, denominator;



	if (!getDisplayAdapters(&numerator, &denominator)) {
		MessageBox(NULL, L"Error gathering display info", L"ERROR", MB_OK);
		return false;
	}

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));


	swapChainDesc.BufferCount = 1; // Back buffer count
	swapChainDesc.BufferDesc.Width = Globals::WINDOW_WIDTH;
	swapChainDesc.BufferDesc.Height = Globals::WINDOW_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (Globals::vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	} else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;


	swapChainDesc.Windowed = !Globals::FULL_SCREEN;


	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;


	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext))) {
		MessageBox(hwnd, L"Error creating Device and Swap Chain. DX11 not installed?", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBufferPtr))) {
		MessageBox(hwnd, L"Could not get pointer to back buffer.", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView))) {
		MessageBox(hwnd, L"Could not create render target view.", L"ERROR", MB_OK);
		return false;
	}
	backBufferPtr->Release();


	/*** INITIALIZE DEPTH STENCIL ***/
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = Globals::WINDOW_WIDTH;
	depthBufferDesc.Height = Globals::WINDOW_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;


	if (FAILED(device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer))) {
		MessageBox(hwnd, L"Could not create depth-stencil buffer.", L"ERROR", MB_OK);
		return false;
	}

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Front-facing pixel stencil behaviour
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Back-facing pixel stencil behaviour
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState))) {
		MessageBox(hwnd, L"Could not create depth-stencil state.", L"ERROR", MB_OK);
		return false;
	}

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// use the depth buffer as a depth-stencil texture.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView))) {
		MessageBox(hwnd, L"Could not create depth-stencil view.", L"ERROR", MB_OK);
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);


	/**** INITIALIZE RASTERIZER  ****/
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(device->CreateRasterizerState(&rasterDesc, &rasterState))) {
		MessageBox(hwnd, L"Could not create rasterizer state.", L"ERROR", MB_OK);
		return false;
	}

	deviceContext->RSSetState(rasterState);

	/**** INITIALIZE VIEWPORT  ****/
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	viewport.Width = (float) Globals::WINDOW_WIDTH;
	viewport.Height = (float) Globals::WINDOW_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);


	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float) Globals::WINDOW_WIDTH / (float) Globals::WINDOW_HEIGHT;

	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect,
		Globals::SCREEN_NEAR, Globals::SCREEN_DEPTH);
	worldMatrix = XMMatrixIdentity();

	orthoMatrix = XMMatrixOrthographicLH((float) Globals::WINDOW_WIDTH, (float) Globals::WINDOW_HEIGHT,
		Globals::SCREEN_NEAR, Globals::SCREEN_DEPTH);

	return true;
}


bool D3D::getDisplayAdapters(UINT* numerator, UINT* denominator) {

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_ADAPTER_DESC adapterDesc;
	unsigned long long stringLength;

	// Create a DirectX graphics interface factory.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory))) {
		MessageBox(NULL, L"Cannot create DXGI factory.", L"ERROR", MB_OK);
		return false;
	}

	int i = 0;
	while (factory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND) {
		adapters.push_back(adapter);
	}
	int size = i - 1;
	i = 0;
	adapter = adapters[0];

	for (int j = 0; j < size; ++j) {
		while (adapter->EnumOutputs(i++, &adapterOutput) != DXGI_ERROR_NOT_FOUND) {

			adapterOutputs.push_back(adapterOutput);
		}
		i = 0;
		adapter = adapters[j];
	}

	adapter = adapters[1];
	adapterOutput = adapterOutputs[0];
	/*if (FAILED(adapter->EnumOutputs(0, &adapterOutput))) {
		MessageBox(NULL, L"Error enumerating outputs.", L"ERROR", MB_OK);
		return false;
	}*/


	// Find total modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) {
		MessageBox(NULL, L"Error enumerating display modes.", L"ERROR", MB_OK);
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList))) {
		MessageBox(NULL, L"Error get display mode list.", L"ERROR", MB_OK);
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int) Globals::WINDOW_WIDTH) {
			if (displayModeList[i].Height == (unsigned int) Globals::WINDOW_HEIGHT) {
				*numerator = displayModeList[i].RefreshRate.Numerator;
				*denominator = displayModeList[i].RefreshRate.Denominator;

			}
		}
	}

	// Get the adapter (video card) description.
	if (FAILED(adapter->GetDesc(&adapterDesc))) {
		MessageBox(NULL, L"Error getting video card description.", L"ERROR", MB_OK);
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	if (wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128) != 0) {
		MessageBox(NULL, L"Error converting name of video card.", L"ERROR", MB_OK);
		return false;
	}

	//delete[] displayModeList;
	//adapterOutput->Release();
	//adapter->Release();
	factory->Release();

	return true;
}


void D3D::getVideoCardInfo(char* cardName, int& memory) {
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
	return;
}

void D3D::shutdownD3D() {

	for (IDXGIAdapter* adapter : adapters)
		adapter->Release();

	for (IDXGIOutput* output : adapterOutputs)
		output->Release();

	if (swapChain) {
		swapChain->SetFullscreenState(false, NULL); // force windowed mode to prevent exception.
	}

	if (rasterState) {
		rasterState->Release();
	}

	if (depthStencilView) {
		depthStencilView->Release();
	}

	if (depthStencilState) {
		depthStencilState->Release();
	}

	if (depthStencilBuffer) {
		depthStencilBuffer->Release();
	}

	if (renderTargetView) {
		renderTargetView->Release();
	}

	if (deviceContext) {
		deviceContext->Release();
	}

	if (device) {
		device->Release();
	}

	if (swapChain) {
		swapChain->Release();
	}
}