#pragma once


#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


#include "Texture.h"

using namespace DirectX;
using namespace std;

class Mesh {
private:
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

		Texture* texture;
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		unsigned int numVerts, numIndices;
	};

	struct ModelData {

		ModelData() {
			numVerts = numIndices = 0;
		}

		string modelName; // filename
		string filepath;
		vector<MeshData> meshData;

		ID3D11Buffer *vertexBuffer = 0;
		ID3D11Buffer *indexBuffer = 0;
		unsigned int numMeshes, numVerts, numIndices;

		void release() {
			if (vertexBuffer)
				vertexBuffer->Release();
			if (indexBuffer)
				indexBuffer->Release();

		}
	};

public:



	Mesh();
	~Mesh();

	//bool Mesh::loadTest(const string fileName);
	bool loadMesh(ID3D11Device* device, const string filename);

	void render(ID3D11DeviceContext* deviceContext);

	int getIndexCount();
private:

	ModelData* modelData;
	//MeshData* meshData;





	bool initFromScene(ID3D11Device* device, const aiScene* scene);

	void loadTextures(MeshData* data, const aiMesh* aiMesh);


	bool initializeBuffers(ID3D11Device* device);

	void ERRORMESSAGE(stringstream& msg);



	void release();
};