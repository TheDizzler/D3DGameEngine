#pragma once

#include "Shader.h"


class TextureShader  : public Shader {
public:
	TextureShader();
	~TextureShader();

	virtual void shutdown();

	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	
	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename);
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

	

	ID3D11SamplerState* sampleState;
	
};

