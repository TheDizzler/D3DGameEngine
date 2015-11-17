#include "GraphicsEngine.h"

#include <tchar.h>
#include <strsafe.h>


GraphicsEngine::GraphicsEngine() {

	bgColor[0] = 1.0f;
	bgColor[1] = 1.0f;
	bgColor[2] = 1.0f;
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
	//camera->setPosition(0.0f, 1.0f, -5.0f);
	camera->setPosition(0.0f, 15.0f, -165.0f);

	/*model = new Model();
	if (!model->initialize(device, "./cube.txt", L"./Assets/seafloor.dds")) {
		MessageBox(NULL, L"Error trying to initialize model", L"ERROR", MB_OK);
		return false;
	}*/

	mesh = new Mesh();
	//if (!mesh->loadMesh(device, "./assets/house/house.obj")) {
	//if (!mesh->loadMesh(device, "./assets/Aphrodite/aphrodite.obj")) {
	if (!mesh->loadMesh(device, "../../D3DEngine/assets/spider/spider.obj")) {
	//if (!mesh->loadMesh(device, "./assets/castle/castle01.obj")) {
		MessageBox(NULL, L"Error trying to initialize mesh", L"ERROR", MB_OK);
		return false;
	}
	/*modelb = new Model();
	if (!modelb->initialize(device, "../cube.txt", L"./assets/seafloor.dds")) {
		MessageBox(NULL, L"Error trying to initialize model", L"ERROR", MB_OK);
		return false;
	}*/
	//modelb->setPosition();

	light = new DiffuseLight();
	light->setAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, 0.0f, 0.0f);

	shaders = new ShaderManager();
	shaders->initializeManager(device, hwnd);


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

float rotation = 0.0f;

void GraphicsEngine::update(double time, int fps) {


	wostringstream ws;
	ws << "FPS: " << fps << "   Time: " << time << "s";
	wstring str(ws.str());
	textFactory->editText(timer, str);

	


	rotation += (float) XM_PI * 0.005f;
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


	mesh->render(deviceContext);
	if (!shaders->lightShader->render(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix, light)) {
		MessageBox(NULL, L"Light Shader malfunction.", L"ERROR", MB_OK);
	}

	//mesh->renderStatic(deviceContext);
	/*if (!lightShader->render(deviceContext, mesh->getIndexCount(), worldMatrix, viewMatrix,
			projectionMatrix, NULL, light->direction, light->diffuseColor))
			MessageBox(NULL, L"Light Shader malfunction.", L"ERROR", MB_OK);*/

	swapChain->Present(Globals::vsync_enabled, 0);

}


void GraphicsEngine::shutdown() {

	if (light)
		delete light;
	if (shaders) 
		shaders->release();
	


	if (mesh)
		delete mesh;
	if (camera)
		delete camera;

	if (textFactory)
		textFactory->release();


	shutdownD3D();

}


