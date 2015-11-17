#pragma once


#include "LightShader.h"
#include "TextureShader.h"
#include "ColorShader.h"
#include "Mesh.h"

class ShaderManager {
public:
	ShaderManager();
	~ShaderManager();

	bool initializeManager(ID3D11Device* device, HWND hwnd);
	void release();

	ColorShader* colorShader = 0;
	LightShader* lightShader = 0;
	TextureShader* textureShader = 0;

	/*bool renderColorShader(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	bool renderTextureShader(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture);

	bool renderLightShader(ID3D11DeviceContext *deviceContext, Mesh* mesh,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		DiffuseLight* light);*/

private:

	const WCHAR* COLOR_VERTEX_SHADER = L"../../D3DEngine/source/shaders/VertexShader.hlsl";
	const WCHAR* COLOR_PIXEL_SHADER = L"../../D3DEngine/source/shaders/PixelShader.hlsl";
	const WCHAR* TEXTURE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/TextureVertexShader.hlsl";
	const WCHAR* TEXTURE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/TexturePixelShader.hlsl";
	const WCHAR* LIGHT_VERTEX_SHADER = L"../../D3DEngine/source/shaders/LightVertexShader.hlsl";
	const WCHAR* LIGHT_PIXEL_SHADER = L"../../D3DEngine/source/shaders/LightPixelShader.hlsl";
	const WCHAR* DIFFUSE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/DiffuseVertexShader.hlsl";
	const WCHAR* DIFFUSE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/DiffusePixelShader.hlsl";
};
