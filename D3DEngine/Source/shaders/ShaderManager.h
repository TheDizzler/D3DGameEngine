#pragma once


#include "LightShader.h"
#include "TextureShader.h"
#include "ColorShader.h"
#include "MeshLoader.h"


/** All Shaders are loaded at runtime for testing purposes. */
class ShaderManager {
public:


	ShaderManager();
	~ShaderManager();

	bool initializeManager(ID3D11Device* device, HWND hwnd);

	void render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]);

	void release();


	//template< class BaseShader >
	//BaseShader* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile);

	BaseShader* baseShader;

	ColorShader* colorShader = 0;
	LightShader* lightShader = 0;
	TextureShader* textureShader = 0;

private:

	vector<BaseShader*> shaders;


	const WCHAR* COLOR_VERTEX_SHADER = L"../../D3DEngine/source/shaders/ColorVertexShader.hlsl";
	const WCHAR* COLOR_PIXEL_SHADER = L"../../D3DEngine/source/shaders/ColorPixelShader.hlsl";
	const WCHAR* TEXTURE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/TextureVertexShader.hlsl";
	const WCHAR* TEXTURE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/TexturePixelShader.hlsl";
	const WCHAR* LIGHT_VERTEX_SHADER = L"../../D3DEngine/source/shaders/LightVertexShader.hlsl";
	const WCHAR* LIGHT_PIXEL_SHADER = L"../../D3DEngine/source/shaders/LightPixelShader.hlsl";
	const WCHAR* DIFFUSE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/DiffuseVertexShader.hlsl";
	const WCHAR* DIFFUSE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/DiffusePixelShader.hlsl";
	const WCHAR* IMP_VERTEX_SHADER = L"../../D3DEngine/source/shaders/ImpVertexShader.hlsl";
	const WCHAR* IMP_PIXEL_SHADER = L"../../D3DEngine/source/shaders/ImpPixelShader.hlsl";
};


