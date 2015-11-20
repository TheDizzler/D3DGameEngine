#pragma once

#include "Model.h"
#include "BaseShader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>



using namespace DirectX;

class MeshLoader {

public:

	MeshLoader();
	~MeshLoader();

	void setShader(BaseShader* shader);
	TRESULTS loadMesh(ID3D11Device* device, const string filename);

	//bool loadMesh(ID3D11Device* device, const string filename);
	//void render(ID3D11DeviceContext* deviceContext);
	//void renderStatic(ID3D11DeviceContext* deviceContext);

	//int getIndexCount();
private:

	BaseShader* shader;

	//ID3D11InputLayout* layout;

	bool initFromScene(ID3D11Device* device, const aiScene* scene, Model* model);
	bool initializeBuffers(ID3D11Device* device, Model::MeshData* meshData);


	//bool initializeBuffers(ID3D11Device* device, MeshData* meshData);
	//bool initializeStaticBuffers(ID3D11Device* device);

	



	void release();
};