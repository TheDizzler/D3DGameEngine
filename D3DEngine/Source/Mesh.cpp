#include "Mesh.h"



Mesh::Mesh() {
}


Mesh::~Mesh() {
}


bool Mesh::loadTest(const string fileName) {
	Assimp::Importer imp;
	const aiScene* scene = NULL;
	const aiMesh* pMesh = NULL;

	scene = imp.ReadFile(fileName, aiProcess_Triangulate);
	if (!scene) {
		MessageBox(NULL, L"Error read file", L"Error", MB_OK);
		return false;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		numVerts += scene->mMeshes[i]->mNumVertices;
		numIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	/*pMesh = pScene->mMeshes[0];
	if (!pMesh) {
		MessageBox(NULL, L"Failed to find meshes", L"Error", MB_OK);
		return false;
	}

	for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
		if (pMesh->mFaces[i].mNumIndices == 3)
			numIndices = numIndices + 3;
		else {
			MessageBox(NULL, L"Failed to parsing faces", L"Error", MB_OK);
			return false;
		}
	}*/

	stringstream sstest;
	sstest << "numVerts: " <<  numVerts << " numIndices: " << numIndices;
	string test = sstest.str();
	wstring wtest;
	for (int i = 0; i < test.size(); ++i)
		wtest += wchar_t(test[i]);
	MessageBox(NULL, wtest.c_str(), L"Seems ok here", MB_OK);

	return true;
}


//bool Mesh::loadMesh(ID3D11Device* device, const string filename) {
//
//	Assimp::Importer assimp;
//
//	const aiScene* scene = assimp.ReadFile(filename,
//		aiProcess_Triangulate // translate models made from non triangle polygons into triangle based meshes
//		| aiProcess_GenSmoothNormals // generates vertex normals if model does not already contain them
//		| aiProcess_FlipUVs // flips texture coordinates along the Y axis
//		/*| aiProcess_CalcTangentSpace
//		| aiProcess_JoinIdenticalVertices
//		| aiProcess_SortByPType
//		| aiProcessPreset_TargetRealtime_MaxQuality
//		| aiProcess_ConvertToLeftHanded*/);
//
//
//
//	if (!scene) {
//		char* error = (char*) assimp.GetErrorString();
//		wchar_t wtext[256];
//		mbstowcs(wtext, error, strlen(error) + 1);
//		MessageBox(NULL, wtext, L"Assimp Error", MB_OK);
//		return false;
//	}
//
//	modelData->meshName = filename;
//
//	if (!initFromScene(scene, filename)) {
//		return false;
//	}
//
//
//
//
//	if (!initializeBuffers(device)) {
//		return false;
//	}
//
//	return true;
//}
//
//
//
//bool Mesh::initFromScene(const aiScene* scene) {
//
//	
//
//	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
//		numVerts += scene->mMeshes[i]->mNumVertices;
//		numIndices += scene->mMeshes[i]->mNumFaces * 3;
//	}
//
//	int stride = (numVerts >= 65536 ? 4 : 2);
//
//	modelData->vertices.resize(numVerts);
//	meshData->indices.resize((stride / 2) * numIndices);
//
//
//	for (int i = 0; i < scene->mNumMeshes; ++i) {
//
//		const aiMesh* mesh = scene->mMeshes[i];
//		initMesh(i, mesh);
//	}
//
//	if (scene->HasTextures())
//		initMaterials(scene);
//
//}
//
//void Mesh::initMesh(unsigned int index, const aiMesh* mesh) {
//
//	vector<Vertex> tempVerts(numVerts);
//	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
//
//	for (int i = 0; i < mesh->mNumVertices; i++) {
//
//		Vertex v;
//		v.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
//		v.texture = XMFLOAT2(mesh->mNormals[i].x, mesh->mNormals[i].y);
//		if (mesh->HasTextureCoords(0))
//			v.normal = XMFLOAT3(mesh->mTextureCoords[0][i].x,
//				mesh->mTextureCoords[0][i].y, mesh->mTextureCoords[0][i].z);
//
//		vertices.push_back(v);
//
//	}
//
//	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//		const aiFace& face = mesh->mFaces[i];
//		assert(face.mNumIndices == 3);
//		indices.push_back(face.mIndices[0]);
//		indices.push_back(face.mIndices[1]);
//		indices.push_back(face.mIndices[2]);
//	}
//
//	entries[index].init(vertices, indices);
//}
//
//
//
//bool Mesh::initializeBuffers(ID3D11Device* device) {
//
//	Vertex* tempVerts;
//	unsigned long* tempIndices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//
//	tempVerts = new Vertex[numVerts];
//	tempIndices = new unsigned long[numIndices];
//
//	for (int i = 0; i < numVerts; ++i) {
//
//		tempVerts[i] = vertices[i];
//		tempIndices[i] = i;
//	}
//
//
//	/**** STATIC VERTEX BUFFER ****/
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(Vertex) * numVerts;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	vertexData.pSysMem = tempVerts;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
//		return false;
//	}
//
//	/**** STATIC INDEX BUFFER ****/
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * numIndices;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	indexData.pSysMem = tempIndices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
//		return false;
//	}
//
//	delete[] tempVerts;
//	delete[] tempIndices;
//
//	return true;
//}
//
//
//
//void Mesh::render(ID3D11DeviceContext* deviceContext) {
//
//	unsigned int stride;
//	unsigned int offset;
//
//	stride = sizeof(Vertex);
//	offset = 0;
//
//	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
//	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//}