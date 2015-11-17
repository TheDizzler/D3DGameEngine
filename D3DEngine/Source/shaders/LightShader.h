#pragma once

#include "ShaderBase.h"
#include "DiffuseLight.h"

class LightShader : public ShaderBase {
private:

	/** I bundled the ambient with the diffuse as their is usually only one of each? */
	struct DiffuseLightBuffer {
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};


public:
	LightShader();
	~LightShader();

	virtual void release();

	bool render(ID3D11DeviceContext *deviceContext, Mesh* mesh, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, DiffuseLight* light);

	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

protected:

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename);
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	
	virtual HRESULT initMatrixBuffer(ID3D11Device* device);
	virtual HRESULT initSamplerState(ID3D11Device* device);
	virtual HRESULT initLightBuffer(ID3D11Device* device);

	bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Mesh* mesh,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor);


	/* Obsolete me thinks */
	bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

	ID3D11Buffer* lightBuffer = 0;
};

