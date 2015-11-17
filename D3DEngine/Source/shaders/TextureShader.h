#pragma once

#include "ShaderBase.h"

/** Probably obsolete now. This is the same as LightShader but without the lighting effects. */
class TextureShader  : public ShaderBase {
public:
	TextureShader();
	~TextureShader();

	virtual void release();

	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

protected:
	
	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename);
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

	virtual HRESULT initMatrixBuffer(ID3D11Device* device);
	virtual HRESULT initSamplerState(ID3D11Device* device);
	
};

