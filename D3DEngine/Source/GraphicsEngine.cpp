#include "GraphicsEngine.h"

#include <tchar.h>
#include <sstream>
#include <strsafe.h>


GraphicsEngine::GraphicsEngine() {

	bgColor[0] = 0.0f;
	bgColor[1] = 0.0f;
	bgColor[2] = 0.0f;
	bgColor[3] = 0.0f;
}


GraphicsEngine::~GraphicsEngine() {

	shutdown();
}

bool GraphicsEngine::initGFXEngine(HINSTANCE hInstance, HWND hwnd) {


	if (!initD3D(hInstance, hwnd)) {
		MessageBox(hwnd, L"Direct3D failed to initialize", L"ERROR", MB_OK);
		return false;
	}


	camera = new Camera();
	camera->setPosition(0.0f, 0.0f, -5.0f);

	model = new Model();
	if (!model->initialize(device, deviceContext, "./Assets/stone01.tga")) {
		MessageBox(NULL, L"Error trying to load model", L"ERROR", MB_OK);
		return false;
	}

	modelb = new Model();
	if (!modelb->initialize(device, deviceContext)) {
		MessageBox(NULL, L"Error trying to load model", L"ERROR", MB_OK);
		return false;
	}
	//modelb->setPosition();

	light = new LightSource();
	light->setDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	light->setDirection(0.0f, 0.0f, 1.0f);

	colorShader = new ColorShader();
	if (!colorShader->initialize(device, hwnd, COLOR_VERTEX_SHADER, COLOR_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Error initializing Color Shader", L"ERROR", MB_OK);
		return false;
	}

	textureShader = new TextureShader();
	if (!textureShader->initialize(device, hwnd, TEXTURE_VERTEX_SHADER, TEXTURE_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Texture Shader object.", L"Error", MB_OK);
		return false;
	}


	lightShader = new LightShader();
	if (!lightShader->initialize(device, hwnd, LIGHT_VERTEX_SHADER, LIGHT_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Light Shader object.", L"Error", MB_OK);
		return false;
	}


	testText();



	return true;
}

/** Displays all display drivers on screen */
void GraphicsEngine::testText() {

	textFactory = new TextFactory(device, deviceContext);

	timer = textFactory->createText(L"FPS", 50, 50, 40);
	TextLabel* displayInfoText = textFactory->createText(L"CardInfo", 50, 150, 20);
	char cardName[128];
	int memory;
	getVideoCardInfo(cardName, memory);

	wostringstream ws1;
	ws1 << "cardName: " << cardName << "   Memory: " << memory << "MBs";
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



void GraphicsEngine::update(double time, int fps) {


	wostringstream ws;
	ws << "FPS: " << fps << "   Time: " << time << "s";
	wstring str(ws.str());
	textFactory->editText(timer, str);

	static float rotation = 0.0f;


	rotation += (float) XM_PI * 0.01f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}
	
	worldMatrix = XMMatrixRotationY(rotation);

}


void GraphicsEngine::render() {

	XMMATRIX viewMatrix;

	deviceContext->ClearRenderTargetView(renderTargetView, bgColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	textFactory->draw();

	camera->render();
	camera->getViewMatrix(viewMatrix);


	/*modelb->render(deviceContext);
	if (!colorShader->render(deviceContext, modelb->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		MessageBox(NULL, L"Color Shader malfunction.", L"ERROR", MB_OK);*/

	model->render(deviceContext);
	/*if (!textureShader->render(deviceContext, model->getIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, model->getTexture()))
		MessageBox(NULL, L"Texture Shader malfunction.", L"ERROR", MB_OK);*/
	if (!lightShader->render(deviceContext, model->getIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, model->getTexture(), light->direction, light->diffuseColor))
		MessageBox(NULL, L"Light Shader malfunction.", L"ERROR", MB_OK);

	// This IF check is being run every frame and could be avoided
	if (Globals::vsync_enabled) {
		// Lock to screen refresh rate.
		swapChain->Present(1, 0);
	} else {
		// Present as fast as possible.
		swapChain->Present(0, 0);
	}
}

// This IF check is being run every frame and could be avoided
//void GraphicsEngine::endDraw() {
//
//	// Present the back buffer to the screen since rendering is complete.
//	if (m_vsync_enabled) {
//		// Lock to screen refresh rate.
//		swapChain->Present(1, 0);
//	} else {
//		// Present as fast as possible.
//		swapChain->Present(0, 0);
//	}
//}




void GraphicsEngine::shutdown() {

	delete light;
	lightShader->shutdown();
	textureShader->shutdown();
	colorShader->shutdown();
	model->shutdown();
	delete camera;

	if (textFactory)
		textFactory->release();


	shutdownD3D();

}


