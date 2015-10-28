#include "Mesh.h"



Mesh::Mesh() {
}


Mesh::~Mesh() {

	release();
}

void Mesh::release() {

}

bool Mesh::loadMesh(ID3D11Device* device, const string& modelFile) {

	Assimp::Importer assimp;

	const aiScene* scene;
	const aiMesh* mesh;

	/** Assimp is able to auto-determine file format! */
	scene = assimp.ReadFile(modelFile,
		aiProcess_Triangulate // translate models made from non triangle polygons into triangle based meshes
		| aiProcess_GenSmoothNormals // generates vertex normals if model does not already contain them
		| aiProcess_FlipUVs // flips texture coordinates along the Y axis
		/*| aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType*/);



	if (!scene) {
		char* error = (char*) assimp.GetErrorString();
		wchar_t wtext[256];
		mbstowcs(wtext, error, strlen(error) + 1);
		MessageBox(NULL, wtext, L"Assimp Error", MB_OK);
		return false;
	}


	return initFromScene(device, scene, modelFile);
}


bool Mesh::initFromScene(ID3D11Device* device, const aiScene* scene, const string& filename) {

	//meshData.resize(scene->mNumMeshes);
	//textures.resize(scene->mNumMaterials);

	MeshData data;

	if (scene->HasAnimations()) {

		aiAnimation* anim = scene->mAnimations[0];

		for (int i = 0; anim->mChannels[0]->mNumPositionKeys; ++i) {

			aiVector3D position = anim->mChannels[0]->mPositionKeys[i].mValue;

			AnimationKey key;
			key.position = XMFLOAT3(position.x, position.y, position.z);

			data.animKeys.push_back(key);
		}

	}


	// Initialize the meshes and materials in the scene
	for (int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		initMesh(mesh, data);
		initMaterials(device, scene, i, data, filename);
	}

	if (!initializeBuffers(device, data, filename))
		return false;

	return true;

}

/** Get textures */
bool Mesh::initMaterials(ID3D11Device* device, const aiScene* scene,
	int index, MeshData data, const string& filename) {

	const aiMaterial* material = scene->mMaterials[index];

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString path;

		Texture* texture = new Texture();

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

			string fullpath = path.data;

			if (fullpath.find("\\") != string::npos)
				fullpath = fullpath.substr(fullpath.find_last_of("\\") + 1,
					fullpath.length() - (fullpath.length() - fullpath.find_last_of("\\") - 1));
			else if (fullpath.find("/") != string::npos)
				fullpath = fullpath.substr(fullpath.find_last_of("//") + 1,
					fullpath.length() - (fullpath.length() - fullpath.find_last_of("//") - 1));

			string rFile = filename;

			if (filename.find("\\") != string::npos)
				rFile = filename.substr(0, filename.find_last_of("\\") + 1);
			else
				rFile = filename.substr(0, filename.find_last_of("/") + 1);

			fullpath = rFile + fullpath;

			wstring wstr = wstring(fullpath.begin(), fullpath.end());
			if (!texture->initialize(device, wstr.c_str()))
				return false;

		} else {
			// Load a white texture in case the model does not include its own texture
			if (texture->initialize(device, L"./Assets/white.png"))
				return false;

			data.textures.push_back(texture);
		}
	}
}



void Mesh::initMesh(const aiMesh* mesh, MeshData data) {

	vector<VertexMesh> vertices;
	vector<DWORD> indices;


	if (mesh->HasPositions()) {
		for (int i = 0; i != mesh->mNumVertices; i++) {

			VertexMesh v;
			v.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			if (mesh->HasTextureCoords(0))
				v.textureCoords = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			if (mesh->HasNormals())
				v.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			/*if (mesh->HasTangentsAndBitangents()) {
			tanx = mesh->mTangents[i].x; tany = mesh->mTangents[i].y; tanz = mesh->mTangents[i].z;
			bitanx = mesh->mBitangents[i].x; bitany = mesh->mBitangents[i].y; bitanz = mesh->mBitangents[i].z;
			}*/

			vertices.push_back(v);
		}

		for (int i = 0; i != mesh->mNumFaces; i++) {
			for (int index = 0; index != mesh->mFaces[i].mNumIndices; index++) {
				indices.push_back(mesh->mFaces[i].mIndices[index]);
			}
		}
	}

	data.meshVertices.push_back(vertices);
	data.indices.push_back(indices);
}

//void Mesh::initMesh(unsigned int index, const aiMesh* paiMesh) {
//
//	entries[index].materialIndex = paiMesh->mMaterialIndex;
//
//	vector<VertexMesh> vertices;
//	vector<unsigned int> indices;
//
//	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
//
//	for (int i = 0; i < paiMesh->mNumVertices; i++) {
//		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
//		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
//		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
//
//		VertexMesh v;
//		v.position = XMFLOAT3(pPos->x, pPos->y, pPos->z);
//		v.texture = XMFLOAT2(pTexCoord->x, pTexCoord->y);
//		v.normal = XMFLOAT3(pNormal->x, pNormal->y, pNormal->z);
//
//		vertices.push_back(v);
//
//	}
//
//	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
//		const aiFace& face = paiMesh->mFaces[i];
//		assert(face.mNumIndices == 3);
//		indices.push_back(face.mIndices[0]);
//		indices.push_back(face.mIndices[1]);
//		indices.push_back(face.mIndices[2]);
//	}
//
//	entries[index].init(vertices, indices);
//}


//bool Mesh::initMaterials(const aiScene* scene, const string& filename) {
//
//	string::size_type slashIndex = filename.find_last_of("/");
//	string dir;
//
//	if (slashIndex == string::npos) {
//		dir = ".";
//	} else if (slashIndex == 0) {
//		dir = "/";
//	} else {
//		dir = filename.substr(0, slashIndex);
//	}
//
//	bool Ret = true;
//
//	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
//		const aiMaterial* pMaterial = scene->mMaterials[i];
//		textures[i] = NULL;
//		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
//			aiString path;
//
//			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
//				std::string FullPath = dir + "/" + path.data;
//				textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
//
//				if (!textures[i]->load()) {
//					printf("Error loading texture '%s'\n", FullPath.c_str());
//					delete textures[i];
//					textures[i] = NULL;
//					Ret = false;
//				}
//			}
//		}
//
//		// Load a white texture in case the model does not include its own texture
//		if (!textures[i]) {
//			textures[i] = new Texture(GL_TEXTURE_2D, "./Assets/white.png");
//
//			Ret = textures[i]->load();
//		}
//	}
//}


bool Mesh::initializeBuffers(ID3D11Device* device, MeshData data, const string& filename) {

	/*for (int i = 0; i < data.animKeys.size(); ++i) {
		data.animKeys[i].position.x *= scalation.x;
		data.animKeys[i].position.y *= scalation.y;
		data.animKeys[i].position.z *= scalation.z;
	}*/

	VertexMesh* vertices;
	DWORD* indices;

	vector<VertexMesh> combinedVertices;
	
	for (int i = 0; i != data.meshVertices.size(); i++) {
		if (i != 0)
			subMeshes.push_back(SubMesh());

		// Prepare the data
		if (i == 0) {// Not an instance
		
			vertices = new VertexMesh[data.meshVertices[i].size()];
			indices = new DWORD[data.indices[i].size()];

			vertices = data.meshVertices[i].ToArray();
			indices = data.indices[i].ToArray();
		} else {
			subMeshes.back().vertices =  new VertexMesh[data.meshVertices[i].size()];
			subMeshes.back().indices = new DWORD[data.indices[i].size()];

			subMeshes.back().vertices = data.meshVertices[i].ToArray();
			subMeshes.back().indices = data.indices[i].ToArray();
		}

		//combinedVertices.insert( combinedVertices.end(), data.meshData[i].group.begin(), data.meshData[i].group.end() );
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexMesh) * data.meshVertices[i].size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.

		if (i == 0)
			vertexData.pSysMem = vertices;
		else
			vertexData.pSysMem = subMeshes[subMeshes.size() - 1].vertices;

		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		if (i == 0) // Not an instance
			device->CreateBuffer(&vertexBufferDesc, &vertexData, &UMeshVBuffer);
		else
			device->CreateBuffer(&vertexBufferDesc, &vertexData, &subMeshes.back().meshVertBuff);

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * data.indices[i].size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.

		if (i == 0)
			indexData.pSysMem = indices;
		else
			indexData.pSysMem = subMeshes[subMeshes.size() - 1].indices;

		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		if (i == 0) // Not an instance
			device->CreateBuffer(&indexBufferDesc, &indexData, &UMeshIBuffer);
		else
			device->CreateBuffer(&indexBufferDesc, &indexData, &subMeshes.back().meshIndexBuff);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (i != NULL) {
			subMeshes.back().passes.RecieveShadows = passes.RecieveShadows;
			subMeshes.back().passes.CastShadows = passes.CastShadows;
			subMeshes.back().passes.Diffuse = passes.Diffuse;
			subMeshes.back().passes.Lighting = passes.Lighting;
			subMeshes.back().passes.doubleSided = passes.doubleSided;
			subMeshes.back().passes.texture_repeat = passes.texture_repeat;
			subMeshes.back().passes.Wind = passes.Wind;
			subMeshes.back().passes.GlobalMaterial.AmbientColor = passes.GlobalMaterial.AmbientColor;
		}

		if (i == NULL) {
			indexCount = data.indices[i].size();
			vertexCount = data.meshVertices[i].size();
		} else {
			subMeshes.back().indexCount = data.indices[i].size();
			subMeshes.back().vertexCount = data.meshVertices[i].size();
		}

		// Animations

		TextureHolder = new TextureObject[data.textures.size()];

		// PrepareTextures if any
		if (TEXTURESTATE) {
			if (data.textures.size() == data.meshVertices.size()) {
				TextureHolder[i].CreateTexture(device, data.textures[i].c_str());

				if (i == NULL) {
					passes.base = &TextureHolder[i];
					passes.Texture = true;
				} else {
					subMeshes.back().passes.base = &TextureHolder[i];
					subMeshes.back().passes.Texture = true;
				}
			}
		}
	}

	generateBoundingBoxes();
}

void Mesh::render(ID3D11DeviceContext *deviceContext) {

	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.

	stride = sizeof(VertexMesh);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &meshVertBuff, &stride, &offset);
	deviceContext->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
