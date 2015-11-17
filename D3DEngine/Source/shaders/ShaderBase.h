#pragma once


#include "Globals.h"
#include "Mesh.h"


using namespace DirectX;
using namespace std;

/** Base class for shaders. This is to make it easier to play with different shader types. */
class ShaderBase {
protected:
	struct ConstantMatrix {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:

	ShaderBase();
	~ShaderBase();

	bool initialize(ID3D11Device* device, HWND hwnd, const WCHAR * vsFilename, const WCHAR * psFilename);
	virtual void release() = 0;


protected:

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename) = 0;
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount) = 0;

	virtual HRESULT initMatrixBuffer(ID3D11Device* device) = 0;
	virtual HRESULT initSamplerState(ID3D11Device* device) = 0;

	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

	ID3D11VertexShader* vertexShader = 0;
	ID3D11PixelShader* pixelShader = 0;
	ID3D11InputLayout* layout = 0;
	/* The Constant/Matrix Buffer */
	ID3D11Buffer* matrixBuffer = 0;
	ID3D11SamplerState* sampleState = 0;
};

