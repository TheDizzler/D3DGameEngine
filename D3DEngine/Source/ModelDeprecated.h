#pragma once


#include "MeshLoader.h"

using namespace DirectX;
using namespace std;


/** Base class for 3D Models */
class Model {
private:


public:
	Model();
	~Model();

	

	bool initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename);
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void shutdown();
	/** Puts the model geometry on the video card to prepare it for drawing by the color shader */
	void render(ID3D11DeviceContext*);

	int getIndexCount();

	ID3D11ShaderResourceView* getTexture();

private:

	bool hasTexture;
	Texture* texture;
	//ModelType* model;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;

	/** Creates Index and Vertex Buffers */
	bool initializeBuffers(ID3D11Device* device);
	bool initializeLightBuffers(ID3D11Device* device);
	bool initializeTextureBuffers(ID3D11Device* device);
	/** Load Texture for old static model. */
	bool loadTGATexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	bool loadModel(char* modelFilename);
	bool loadTexture(ID3D11Device* device, WCHAR* textureFilename);


	
};

