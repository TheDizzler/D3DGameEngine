#pragma once
#include <Effects.h>
//#include <GeometricPrimitive.h>

#include "Globals.h"
#include "stuff.h"
#include "PrimitiveGeometry.h"

#include "Camera.h"

using namespace std;
using namespace DirectX;

/* A simple model with only one texture. */
class BasicModel {
public: // move these

	// A structure to hold the data for a per-object constant buffer
	// defined in the vertex shader.
	struct PerFrameConstantBufferData {
		XMMATRIX viewProjectionMatrix;
	};
	// Per-Frame constant buffer defined in the instanced vertex shader.
	Microsoft::WRL::ComPtr<ID3D11Buffer> perFrameConstantBuffer;
	

	// This structure is used in the simple vertex shader.
	struct PerObjectConstantBufferData {
		XMMATRIX WorldMatrix;
		XMMATRIX InverseTransposeWorldMatrix;
		XMMATRIX WorldViewProjectionMatrix;
	};
	// Per-Object constant buffer defined in the simple vertex shader.
	Microsoft::WRL::ComPtr<ID3D11Buffer> perObjectConstantBuffer;

	// Light properties defined in the pixel shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightPropertiesConstantBuffer;
	LightProperties lightProperties;


	// Vertex shader for instanced rendering.
	Microsoft::WRL::ComPtr<ID3D11VertexShader> instancedVertexShader;
	// Vertex shader for simple (non-instanced) geometry.
	Microsoft::WRL::ComPtr<ID3D11VertexShader> simpleVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> texturedLitPixelShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> instancedInputLayout;

	const WCHAR* SIMPLE_VERTEX_SHADER = L"../../D3DEngine/source/shaders/SimplevertexShader.hlsl";
	const WCHAR* INSTANCED_VERTEX_SHADER = L"../../D3DEngine/source/shaders/InstancedVertexShader.hlsl";
	const WCHAR* TEXTURED_LIT_PIXEL_SHADER = L"../../D3DEngine/source/shaders/TexturedLitPixelShader.hlsl";

	
	HRESULT initSamplerState(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tempTexture;


	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd,
		const WCHAR* shaderFilename);
	/*** Everything above should be moved into their relevant classes. ****/

public:
	struct VertexPosNormTex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 textureCoords;
	};

	__declspec(align(16)) struct InstanceData {
		XMMATRIX worldMatrix;
		XMMATRIX inverseTransposeWorldMatrix;
	};


	BasicModel(ID3D11Device * device);
	~BasicModel();

	void update(float elapsedTime, Camera camera, ID3D11DeviceContext* deviceContext);

	void render(ID3D11DeviceContext* deviceContext, Camera camera,
		ID3D11RasterizerState* rasterizerState, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilState* depthStencilState, ID3D11DepthStencilView* depthStencilView);

	bool loadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const WCHAR* filename);

private:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	// The instance buffer contains the per-instance data that will be used
	// to render the 6 walls of our room.
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
	// Material properties defined in the pixel shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> materialPropertiesConstantBuffer;


	// Vertices for a unit plane.
	VertexPosNormTex planeVerts[4] = {
		{XMFLOAT3(-0.5f, 0.0f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)}, // 0
		{XMFLOAT3(0.5f, 0.0f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)}, // 1
		{XMFLOAT3(0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)}, // 2
		{XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)}  // 3
	};
	int numInstances = 6;
	// Index buffer for the unit plane.
	WORD planeIndex[6] = {
		0, 1, 3, 1, 2, 3
	};

	// Create some geometric primitives for the scene.
	// The outer walls of our room.
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_d3dVertexPositionNormalTextureInputLayout;
	unique_ptr<PrimitiveGeometry> sphere;
	unique_ptr<PrimitiveGeometry> cube;
	unique_ptr<PrimitiveGeometry> cone;
	unique_ptr<PrimitiveGeometry> torus;


	unique_ptr<EffectFactory> effectFactory;
	vector<MaterialProperties> materialProperties;
	
	bool initializeBuffers(ID3D11Device* device);
};