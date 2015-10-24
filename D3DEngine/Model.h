#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include "Texture.h"

using namespace DirectX;


/** Base class for 3D Models */
class Model {
private:
	struct VertexColor {
		// this typedef must match layout in ColorShader
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct VertexTexture {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	Model();
	~Model();

	bool initialize(ID3D11Device*, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void shutdown();
	/** Puts the model geometry on the video card to prepare it for drawing by the color shader */
	void render(ID3D11DeviceContext*);

	int getIndexCount();

private:

	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;

	/** Creates Index and Vertex Buffers */
	bool initializeBuffers(ID3D11Device* device);
	void shutdownBuffers();
	void renderBuffers(ID3D11DeviceContext* deviceContext);


public:

	ID3D11ShaderResourceView* getTexture();


private:

	Texture* texture;
	bool initializeTextureBuffers(ID3D11Device* device);
	bool loadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void releaseTexture();

	//void renderTextureBuffers(ID3D11DeviceContext* deviceContext);
	
};

