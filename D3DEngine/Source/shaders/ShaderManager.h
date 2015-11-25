#pragma once


#include "old/LightShader.h"
#include "old/TextureShader.h"
#include "old/ColorShader.h"
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


	/*const WCHAR* COLOR_VERTEX_SHADER = L"../../D3DEngine/source/shaders/old/ColorVertexShader.hlsl";
	const WCHAR* COLOR_PIXEL_SHADER = L"../../D3DEngine/source/shaders/old/ColorPixelShader.hlsl";
	const WCHAR* TEXTURE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/old/TextureVertexShader.hlsl";
	const WCHAR* TEXTURE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/old/TexturePixelShader.hlsl";
	const WCHAR* LIGHT_VERTEX_SHADER = L"../../D3DEngine/source/shaders/old/LightVertexShader.hlsl";
	const WCHAR* LIGHT_PIXEL_SHADER = L"../../D3DEngine/source/shaders/old/LightPixelShader.hlsl";
	const WCHAR* DIFFUSE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/old/DiffuseVertexShader.hlsl";
	const WCHAR* DIFFUSE_PIXEL_SHADER = L"../../D3DEngine/source/shaders/old/DiffusePixelShader.hlsl";
	const WCHAR* IMP_VERTEX_SHADER = L"../../D3DEngine/source/shaders/ImpVertexShader.hlsl";
	const WCHAR* IMP_PIXEL_SHADER = L"../../D3DEngine/source/shaders/ImpPixelShader.hlsl";*/

	const WCHAR* SIMPLE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/SimplevertexShader.hlsl";
	const WCHAR* INSTANCED_VERTEX_SHADER = L"../../D3DEngine/source/shaders/InstancedVertexShader.hlsl";
	const WCHAR* TEXTURED_LIT_PIXEL_SHADER = L"../../D3DEngine/source/shaders/TexturedLitPixelShader.hlsl";
};


