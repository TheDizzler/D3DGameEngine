#pragma once

#include "Texture.h"

//using namespace DirectX;
using namespace std;


/** Base class for 3D Models */
class Model {
public:
	struct VertexPosNormTexTan {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 textureCoords;
		XMFLOAT3 tangent;
	};

	struct VertexPosNormTex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 textureCoords;
	};

	__declspec(align(16)) struct InstanceData {
		XMMATRIX worldMatrix;
		XMMATRIX inverseTransposeMatrix;
	};

	struct MeshData {

		MeshData() {
			numVerts = numIndices = 0;
		}

		bool hasTexture = false;
		Texture texture;
		vector<VertexPosNormTex> vertices;
		vector<unsigned int> indices;

		unsigned int numVerts, numIndices;

		ID3D11Buffer* vertexBuffer = 0;
		ID3D11Buffer* indexBuffer = 0;

		unsigned int stride = sizeof(VertexPosNormTex);
		unsigned int offset = 0;

		void render(ID3D11DeviceContext* deviceContext);

		void release();
	};


public:


	Model();
	~Model();

	string modelName; // filename
	string filepath;
	vector<MeshData> meshData;

	XMMATRIX worldMatrix;

	unsigned int numMeshes = 0, numTotalVerts = 0, numTotalIndices = 0;

	void render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]);


private:

	

};
