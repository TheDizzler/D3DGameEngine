#pragma once


#include "Globals.h"
#include "Model3D.h"


/* Description of shader to create. My attempt at a Microsoft code style :P. */
typedef struct ShaderDesc {
	ShaderDesc(const WCHAR* vsFilename, LPCSTR vsEntryPoint,
		const WCHAR* psFilename, LPCSTR psEntryPoint) :
		vsFile(vsFilename), vsEP(vsEntryPoint), psFile(psFilename), psEP(psEntryPoint) {

	}
	const WCHAR* vsFile;
	LPCSTR vsEP;
	const WCHAR* psFile;
	LPCSTR psEP;
};

/* These shaders will render per model (ie loads one model and renders all materials on that model).
	Later I might implement per material shader (loads materials and then renders each mesh that uses
	that material). */
class BaseShader {
public:
	// A structure to hold the data for a per-object constant buffer
	// defined in the vertex shader.
	struct PerFrameConstantBufferData {
		XMMATRIX viewProjectionMatrix;
	};

	BaseShader();
	~BaseShader();

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const ShaderDesc* shaderDesc);

	virtual void loadModel(Model* model);

	virtual void render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]);

	void release();

	vector<Model*> models;


protected:

	ID3D11VertexShader* vertexShader = 0;
	ID3D11PixelShader* pixelShader = 0;
	ID3D11InputLayout* layout = 0;
	/* The Constant/Matrix Buffer */
	ID3D11Buffer* matrixBuffer = 0;
	ID3D11Buffer* lightBuffer = 0;
	ID3D11SamplerState* samplerState = 0;

	virtual HRESULT initInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob);
	virtual HRESULT initMatrixBuffer(ID3D11Device* device);
	virtual HRESULT initSamplerState(ID3D11Device* device);
	virtual HRESULT initLightBuffer(ID3D11Device* device);

	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);
};