#pragma once

#include "Shader.h"

/** A shader class for creating geometry with no textures. */
class ColorShader : public Shader {
public:
	ColorShader();
	~ColorShader();

	virtual void shutdown();

	bool render(ID3D11DeviceContext *deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:

	virtual bool initializeShader(ID3D11Device* device, HWND hwnd,
		const WCHAR* vsFilename, const WCHAR* psFilename);
	virtual void renderShader(ID3D11DeviceContext* deviceContext, int indexCount);


	bool setShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
};
