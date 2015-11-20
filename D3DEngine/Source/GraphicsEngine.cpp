#include "GraphicsEngine.h"

#include <tchar.h>
#include <strsafe.h>


GraphicsEngine::GraphicsEngine() {

	clearColor[0] = 1.0f;
	clearColor[1] = 1.0f;
	clearColor[2] = 1.0f;
	clearColor[3] = 0.0f;
}


GraphicsEngine::~GraphicsEngine() {

	shutdown();
}

bool GraphicsEngine::initGFXEngine(HINSTANCE hInstance, HWND hwnd) {


	if (!initD3D(hInstance, hwnd)) {
		MessageBox(hwnd, L"Direct3D failed to initialize", L"ERROR", MB_OK);
		return false;
	}

	if (!createConstantBuffer()) {
		MessageBox(hwnd, L"Direct3D failed to create the constant buffer", L"ERROR", MB_OK);
		return false;
	}

	/**** INITIALIZE VIEWPORT  ****/
	initializeViewPort();


	//worldMatrix = XMMatrixIdentity();

	/*orthoMatrix = XMMatrixOrthographicLH((float) Globals::WINDOW_WIDTH, (float) Globals::WINDOW_HEIGHT,
		Globals::SCREEN_NEAR, Globals::SCREEN_DEPTH);*/

	



	shaderManager = new ShaderManager();
	shaderManager->initializeManager(device, hwnd);
	meshLoader = new MeshLoader();
	meshLoader->setShader(shaderManager->baseShader);


	if (meshLoader->loadMesh(device, "../../D3DEngine/assets/spider/spider.obj") != NO_WORRIES) {
		//MessageBox(NULL, L"Error trying to load mesh", L"ERROR", MB_OK);
		return false;
	}


	//if (!mesh->loadMesh(device, "./assets/house/house.obj")) {
	//if (!mesh->loadMesh(device, "./assets/Aphrodite/aphrodite.obj")) {
	//if (!mesh->loadMesh(device, "./assets/castle/castle01.obj")) {
		//MessageBox(NULL, L"Error trying to initialize mesh", L"ERROR", MB_OK);
		//return false;
	//}

	camera = new Camera();
	//camera->setPosition(0.0f, 1.0f, -5.0f);
	camera->setPosition(0.0f, 15.0f, -165.0f);

	light = new DiffuseLight();
	light->setAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, 0.0f, 0.0f);

	


	testText();



	return true;
}


bool GraphicsEngine::createConstantBuffer() {

	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffers[ApplicationBuffer]))) {
		OutputDebugStringA("Failed creating application constant buffer");
		return false;
	}

	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffers[PerFrameBuffer]))) {
		OutputDebugStringA("Failed creating frame constant buffer");
		return false;
	}

	if (FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffers[PerObjectBuffer]))) {
		OutputDebugStringA("Failed creating object constant buffer");
		return false;
	}

	return true;
}


void GraphicsEngine::initializeViewPort() {

	D3D11_VIEWPORT viewport;
	float screenAspect;

	viewport.Width = (float) Globals::WINDOW_WIDTH;
	viewport.Height = (float) Globals::WINDOW_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);


	screenAspect = (float) Globals::WINDOW_WIDTH / (float) Globals::WINDOW_HEIGHT;

	projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), screenAspect,
		Globals::SCREEN_NEAR, Globals::SCREEN_DEPTH);
	deviceContext->UpdateSubresource(constantBuffers[ApplicationBuffer], 0, NULL, &projectionMatrix, 0, 0);

}


/** Displays all display drivers on screen */
void GraphicsEngine::testText() {

	textFactory = new TextFactory(device, deviceContext);

	timer = textFactory->createText(L"FPS", 50, 50, 40);
	TextLabel* displayInfoText = textFactory->createText(L"CardInfo", 50, 150, 20);
	char cardName[128];
	int memory;
	getVideoCardInfo(cardName, memory);

	wchar_t* feats;
	switch (featureLevel) {
		case 0x9100:
			feats = L"D3D_FEATURE_LEVEL_9_1";
			break;
		case 0x9200:
			feats = L"D3D_FEATURE_LEVEL_9_2";
			break;
		case 0x9300:
			feats = L"D3D_FEATURE_LEVEL_9_3";
			break;
		case 0xa000:
			feats = L"D3D_FEATURE_LEVEL_10_0";
			break;
		case 0xa100:
			feats = L"D3D_FEATURE_LEVEL_10_1";
			break;
		case 0xb000:
			feats = L"D3D_FEATURE_LEVEL_11_0";
			break;
		case 0xb100:
			feats = L"D3D_FEATURE_LEVEL_11_1";
			break;
		case 0xc000:
			feats = L"D3D_FEATURE_LEVEL_12_0";
			break;
		case 0xc100:
			feats = L"D3D_FEATURE_LEVEL_12_1";
			break;
		default:
			feats = L"No D3D Feature level detected?!";
			break;

	}

	wostringstream ws1;
	ws1 << "cardName: " << cardName << " Memory: " << memory << "MBs" << " featureLevel: " << feats;
	wstring str(ws1.str());
	textFactory->editText(displayInfoText, str);


	int i = 0;
	for (IDXGIAdapter* adapter : adapters) {

		TextLabel* label = textFactory->createText(L"Nashi", 50, 175 + i++ * 20, 15);
		DXGI_ADAPTER_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
		adapter->GetDesc(&desc);
		wostringstream ws;
		ws << "Description: " << desc.Description << " DeviceId: " << desc.DeviceId << " Mem: " << desc.DedicatedVideoMemory / 1024 / 1024 << "MBs VendorId: " << desc.VendorId;
		str.assign(ws.str());
		textFactory->editText(label, str);
	}

	++i;
	for (IDXGIOutput* output : adapterOutputs) {

		TextLabel* label = textFactory->createText(L"Nashi", 50, 175 + i++ * 20, 15);
		DXGI_OUTPUT_DESC desc1;
		ZeroMemory(&desc1, sizeof(DXGI_OUTPUT_DESC));
		output->GetDesc(&desc1);
		wostringstream ws2;
		ws2 << " DeviceName: " << desc1.DeviceName << " AttachedToDesktop: " << desc1.AttachedToDesktop << " Monitor: " << desc1.Monitor;
		str.assign(ws2.str());
		textFactory->editText(label, str);
	}

	++i;
	TextLabel* label = textFactory->createText(L"Nashi", 50, 175 + i++ * 20, 15);
	wostringstream modes;
	modes << "numModes: " << numModes << "\n";
	for (int j = 0; j < numModes; j++) {


		modes << "Format: " << displayModeList[j].Format << " Width: " << displayModeList[j].Width << " Height: " << displayModeList[j].Height << " ";
		if (j % 4 == 3)
			modes << "\n";

	}
	str.assign(modes.str());
	textFactory->editText(label, str);
}

float rotation = 0.0f;

void GraphicsEngine::update(double time, int fps) {


	wostringstream ws;
	ws << "FPS: " << fps << "   Time: " << time << "s";
	wstring str(ws.str());
	textFactory->editText(timer, str);

	XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
	XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	deviceContext->UpdateSubresource(constantBuffers[PerFrameBuffer], 0, nullptr, &viewMatrix, 0, 0);


	static float angle = 0.0f;
	angle += 90.0f * time;
	XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

	worldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
	deviceContext->UpdateSubresource(constantBuffers[PerObjectBuffer], 0, nullptr, &worldMatrix, 0, 0);


	camera->setViewMatrix(viewMatrix);
}


void GraphicsEngine::render() {


	deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
	deviceContext->ClearDepthStencilView(depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);

	textFactory->draw();

	camera->render();
	


	
	if (!shaders->lightShader->render(deviceContext, meshLoader, worldMatrix, camera->getViewMatrix() , projectionMatrix, light)) {
		MessageBox(NULL, L"Light Shader malfunction.", L"ERROR", MB_OK);
	}
	meshLoader->render(deviceContext);
	//mesh->renderStatic(deviceContext);
	/*if (!lightShader->render(deviceContext, mesh->getIndexCount(), worldMatrix, viewMatrix,
			projectionMatrix, NULL, light->direction, light->diffuseColor))
			MessageBox(NULL, L"Light Shader malfunction.", L"ERROR", MB_OK);*/

	swapChain->Present(Globals::vsync_enabled, 0);

}


void GraphicsEngine::shutdown() {

	if (light)
		delete light;

	if (shaders) {
		shaders->release();
		delete shaders;
	}



	if (meshLoader)
		delete meshLoader;
	if (camera)
		delete camera;

	if (textFactory) {
		textFactory->release();
		delete textFactory;
	}

	shutdownD3D();

}


