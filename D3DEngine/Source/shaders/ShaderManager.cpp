#include "ShaderManager.h"

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
}


bool ShaderManager::initializeManager(ID3D11Device* device, HWND hwnd) {

	
	ShaderDesc* shaderDesc = new ShaderDesc(IMP_VERTEX_SHADER, "ImpVertexShader",
		IMP_PIXEL_SHADER, "ImpPixelShader");
	

	baseShader = new BaseShader();
	if (!baseShader->initializeShader(device, hwnd, shaderDesc)) {
		ErrorMessage(string("Could not initialize Shader"));
		delete shaderDesc;
		return false;
	}

	shaders.push_back(baseShader);

	
	/*lightShader = new LightShader();
	if (!lightShader->initialize(device, hwnd, DIFFUSE_VERTEX_SHADER, DIFFUSE_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Light Shader object.", L"Error", MB_OK);
		return false;
	}*/

	/*textureShader = new TextureShader();
	if (!textureShader->initialize(device, hwnd, TEXTURE_VERTEX_SHADER, TEXTURE_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Texture Shader object.", L"Error", MB_OK);
		return false;
	}


	colorShader = new ColorShader();
	if (!colorShader->initialize(device, hwnd, COLOR_VERTEX_SHADER, COLOR_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Error initializing Color Shader", L"ERROR", MB_OK);
		return false;
	}*/

	


	delete shaderDesc;

	return true;
}

void ShaderManager::render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]) {

	deviceContext->VSSetConstantBuffers(1, 1, &constantBuffers[PerFrameBuffer]);

	for each (BaseShader* shader in shaders) {
		/*QuickMessage(L"check in Manager");*/
		shader->render(deviceContext, constantBuffers);
	}

}

void ShaderManager::release() {

	if (colorShader)
		colorShader->release();
	if (lightShader)
		lightShader->release();
	if (textureShader)
		textureShader->release();

	for each (BaseShader* shader in shaders) {
		shader->release();
	}
}

//bool ShaderManager::renderColorShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
//	return colorShader->render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
//}
//
//bool ShaderManager::renderTextureShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture) {
//	return textureShader->render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);;
//}
//
//bool ShaderManager::renderLightShader(ID3D11DeviceContext * deviceContext, Mesh * mesh, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, DiffuseLight * light) {
//	return lightShader->render(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix, light);
//}
