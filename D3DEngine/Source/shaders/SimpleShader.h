#pragma once

#include "BaseShader.h"
#include "BasicModel.h"

/* A single instance Shader. */
class SimpleShader : public BaseShader {
public:
	struct PerObjectConstantBufferData {
		XMMATRIX worldMatrix;
		XMMATRIX inverseTransposeWorldMatrix;
		XMMATRIX worldViewProjectionMatrix;
	};


	SimpleShader();
	~SimpleShader();

	virtual void loadModel(Model* model);

	virtual void render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]);


private:

	vector<BasicModel> basicModels;

	

	virtual HRESULT initInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob);
	virtual HRESULT initMatrixBuffer(ID3D11Device* device);
	virtual HRESULT initSamplerState(ID3D11Device* device);
	virtual HRESULT initLightBuffer(ID3D11Device* device);
};