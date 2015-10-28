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
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct MeshData {

		vector<Vertex> vertices;
		vector<unsigned int> indices;

	};

	struct ModelData {

		string meshName; // filename
		MeshData* meshdata;
	};

public:
	Mesh();
	~Mesh();

	bool Mesh::loadTest(const string fileName);

	bool loadMesh(ID3D11Device* device, const string filename);

	void render(ID3D11DeviceContext* deviceContext);

private:

	ModelData* modelData;


	ID3D11Buffer *vertexBuffer, *indexBuffer;
	unsigned int numVerts, numIndices;
	

	bool initFromScene(const aiScene* scene);

	void initMesh(unsigned int index, const aiMesh* mesh);


	bool initializeBuffers(ID3D11Device* device);
};