#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "Globals.h"

using namespace DirectX;
using namespace std;

class Shader {
protected:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:

	Shader();
	~Shader();

	bool initialize(ID3D11Device* device, HWND hwnd, const WCHAR * vsFilename, const WCHAR * psFilename);
	virtual void shutdown() = 0;
	
	
protected:

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename) = 0;
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount) = 0;

	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
};

