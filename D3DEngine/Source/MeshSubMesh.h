#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"


using namespace DirectX;
using namespace std;


class Mesh {

private:
	struct VertexMesh {
		XMFLOAT3 position;
		XMFLOAT2 textureCoords;
		XMFLOAT3 normal;
	};

	struct SubMesh {

		VertexMesh* vertices;
		DWORD* indices;

		ID3D11Buffer* meshVertBuff;
		ID3D11Buffer* meshIndexBuff;
	};

public:

	Mesh();
	~Mesh();

	bool loadMesh(ID3D11Device* device, const string& modelFile);
	void render(ID3D11DeviceContext *deviceContext);

	void release();

private:

	/*struct MeshEntry {


		void init(const vector<VertexMesh>& vertices,
			const vector<unsigned int>& indices) {

			numIndices = indices.size();

		};

		ID3D11Buffer VB = 0;
		ID3D11Buffer IB = 0;
		unsigned int numIndices = 0;
		unsigned int materialIndex;
	};*/

	struct AnimationKey {

		XMFLOAT3 position;

	};

	struct MeshData {

		vector<vector<VertexMesh> > meshVertices;
		vector<vector<DWORD> > indices;
		vector<AnimationKey> animKeys;
		vector<Texture*> textures;
	};

	

	//ID3D11Buffer *vertexBuffer, *indexBuffer;

	/*ID3D11BlendState* transparency;

	ID3D11Buffer* meshVertBuff;
	ID3D11Buffer* meshIndexBuff;
	XMMATRIX meshWorld;
	int meshSubsets = 0;
	vector<int> meshSubsetIndexStart;
	vector<int> meshSubsetTexture;

	vector<ID3D11ShaderResourceView*> meshSRV;
	vector<wstring> textureNameArray;*/

	vector<SubMesh> subMeshes;
	

	bool initFromScene(ID3D11Device* device, const aiScene* pScene, const string& filename);
	void initMesh(const aiMesh* paiMesh, MeshData data);
	bool initMaterials(ID3D11Device* device, const aiScene* scene,
		int index, MeshData data, const string& filename);
	bool initializeBuffers(ID3D11Device* device, MeshData data, const string& filename);


	//bool initMaterials(const aiScene* pScene, const string& filename);
	

	

	
};

