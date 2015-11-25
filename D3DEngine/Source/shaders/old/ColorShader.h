#pragma once

#include "ShaderBase.h"

/** A shader class for creating geometry with no textures. */
class ColorShader : public ShaderBase {
public:
	ColorShader();
	~ColorShader();

	virtual void release();

	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

protected:

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename);

	
	virtual HRESULT initMatrixBuffer(ID3D11Device* device);
	/** Color Shaders do not use sampler state (no textures). */
	virtual HRESULT initSamplerState(ID3D11Device* device);

	
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);


	bool setShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
};
