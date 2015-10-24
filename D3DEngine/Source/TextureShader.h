#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "Globals.h"

using namespace DirectX;
using namespace std;


class TextureShader {
private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	TextureShader();
	~TextureShader();

	bool initialize(ID3D11Device*, HWND);
	void shutdown();
	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	bool initializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void shutdownShader();
	void outputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);
	void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;

	ID3D11SamplerState* sampleState;
};

