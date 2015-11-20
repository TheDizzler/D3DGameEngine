#pragma once

#include "MeshLoader.h"
#include "Globals.h"


/* These shaders will render per model (ie loads one model and renders all materials on that model).
	Later I might implement per material shader (loads materials and then renders each mesh that uses
	that material). */
class BaseShader {
public:

	/* Description of shader to create. My attempt at a Microsoft style coding :P. */
	struct ShaderDesc {
		const WCHAR vsFilename;
		const LPCSTR vsEntryPoint;
		const WCHAR psFilename;
		const LPCSTR psEntryPoint;
	};

	BaseShader();
	~BaseShader();

	bool initializeShader(ID3D11Device* device, HWND hwnd,
		const ShaderDesc* shaderDesc);

	void loadModel(Model* model);

	void render(ID3D11DeviceContext* deviceCon);

	void release();

private:

	vector<Model> models;

	ID3D11VertexShader* vertexShader = 0;
	ID3D11PixelShader* pixelShader = 0;
	ID3D11InputLayout* layout = 0;
	/* The Constant/Matrix Buffer */
	ID3D11Buffer* matrixBuffer = 0;
	ID3D11Buffer* lightBuffer = 0;
	ID3D11SamplerState* sampleState = 0;


	HRESULT initInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob);
	HRESULT initMatrixBuffer(ID3D11Device* device);
	HRESULT initSamplerState(ID3D11Device* device);
	HRESULT initLightBuffer(ID3D11Device* device);

	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);
};