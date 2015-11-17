#include "ShaderManager.h"

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
}


bool ShaderManager::initializeManager(ID3D11Device * device, HWND hwnd) {

	lightShader = new LightShader();
	if (!lightShader->initialize(device, hwnd, DIFFUSE_VERTEX_SHADER, DIFFUSE_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Light Shader object.", L"Error", MB_OK);
		return false;
	}


	textureShader = new TextureShader();
	if (!textureShader->initialize(device, hwnd, TEXTURE_VERTEX_SHADER, TEXTURE_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Could not initialize the Texture Shader object.", L"Error", MB_OK);
		return false;
	}


	colorShader = new ColorShader();
	if (!colorShader->initialize(device, hwnd, COLOR_VERTEX_SHADER, COLOR_PIXEL_SHADER)) {
		MessageBox(hwnd, L"Error initializing Color Shader", L"ERROR", MB_OK);
		return false;
	}

	


	

	return true;
}


void ShaderManager::release() {

	if (colorShader)
		colorShader->release();
	if (lightShader)
		lightShader->release();
	if (textureShader)
		textureShader->release();
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
