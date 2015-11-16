#pragma once


#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Utils.h"
#include "Texture.h"

using namespace DirectX;
//using namespace std;

class Mesh {
public:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 textureCoords;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
	};

	struct MeshData {

		MeshData() {
			numVerts = numIndices = 0;
		}

		/*~MeshData() {
			release();
		}*/

		Texture* texture = 0;
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		unsigned int numVerts, numIndices;

		ID3D11Buffer *vertexBuffer = 0;
		ID3D11Buffer *indexBuffer = 0;

		void release() {
			if (vertexBuffer)
				vertexBuffer->Release();
			if (indexBuffer)
				indexBuffer->Release();

		}
	};

	struct ModelData {

		ModelData() {
			numVerts = numIndices = 0;
		}

		string modelName; // filename
		string filepath;
		vector<MeshData> meshData;

		//int stride = 2;

		/** Buffers for static models */
		ID3D11Buffer *vertexStaticBuffer = 0;
		ID3D11Buffer *indexStaticBuffer = 0;
		unsigned int numMeshes, numVerts, numIndices;

		void release() {
			if (vertexStaticBuffer)
				vertexStaticBuffer->Release();
			if (indexStaticBuffer)
				indexStaticBuffer->Release();

		}
	};

public:

	ModelData* modelData;

	Mesh();
	~Mesh();

	//bool Mesh::loadTest(const string fileName);
	bool loadMesh(ID3D11Device* device, const string filename);

	void render(ID3D11DeviceContext* deviceContext);
	void renderStatic(ID3D11DeviceContext* deviceContext);

	int getIndexCount();
private:

	


	bool initFromScene(ID3D11Device* device, const aiScene* scene);

	bool initializeBuffers(ID3D11Device* device, MeshData* meshData);
	bool initializeStaticBuffers(ID3D11Device* device);

	



	void release();
};