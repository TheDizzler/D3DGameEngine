#include "MeshLoader.h"


MeshLoader::MeshLoader() {
}


MeshLoader::~MeshLoader() {


}

void MeshLoader::setShader(BaseShader* shdr) {

	shader = shdr;
}

TRESULTS MeshLoader::loadMesh(ID3D11Device* device, const string filename) {

	if (!shader) {
		MessageBox(NULL, L"No shader loaded: unable to load Meshes", L"CRITICAL ERROR", MB_OK);
		return PROBLEMS;
	}

	Assimp::Importer assimp;
	const aiScene* scene = NULL;


	scene = assimp.ReadFile(filename,
		aiProcess_Triangulate // translate models made from non triangle polygons into triangle based meshes
		| aiProcess_GenSmoothNormals // generates vertex normals if model does not already contain them
		| aiProcess_FlipUVs // flips texture coordinates along the Y axis
		| aiProcess_CalcTangentSpace
		| aiProcess_PreTransformVertices
		| aiProcess_FixInfacingNormals
		| aiProcess_FindInvalidData
		| aiProcess_ValidateDataStructure
		//| aiProcess_ConvertToLeftHanded
		//| aiProcess_MakeLeftHanded
		//| aiProcess_JoinIdenticalVertices
		//| aiProcess_SortByPType
		//| aiProcessPreset_TargetRealtime_MaxQuality
		//| aiProcess_LimitBoneWeights
		//| aiProcess_OptimizeMeshes
		);


	if (!scene) {
		char* error = (char*) assimp.GetErrorString();
		wchar_t wtext[256];
		mbstowcs(wtext, error, strlen(error) + 1);
		MessageBox(NULL, wtext, L"Assimp Error", MB_OK);
		return PROBLEMS;
	}

	size_t loc = filename.find_last_of('/');


	Model* model = new Model();
	model->modelName = filename.substr(loc + 1);
	model->filepath = filename.substr(0, loc + 1);
	model->numMeshes = scene->mNumMeshes;
	model->meshData.resize(model->numMeshes);


	if (!initFromScene(device, scene, model)) {
		delete model;
		return PROBLEMS;
	}

	shader->loadModel(model);

	return NO_WORRIES;
}




bool MeshLoader::initFromScene(ID3D11Device* device, const aiScene* scene, Model* model) {


	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		model->numTotalVerts += scene->mMeshes[i]->mNumVertices;
		model->numTotalIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	//int stride = (modelData->numVerts >= 65536 ? 4 : 2);


	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {

		const aiMesh* mesh = scene->mMeshes[i];

		Model::MeshData data = Model::MeshData();
		data.numVerts = mesh->mNumVertices;
		data.numIndices = mesh->mNumFaces * 3;
		data.vertices.resize(mesh->mNumVertices);

		if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {

			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			if (mesh->mPrimitiveTypes == aiPrimitiveType_LINE)
				str << " \n Problem: The mesh containes lines when it should only contain triangles";
			else
				str << " \n Problem: The mesh containes points when it should only contain triangles";

			ErrorMessage(str.str());

			continue;
		}

		if (!mesh->HasTextureCoords(0)) {
			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			str << " \n Problem: The mesh containes no texcoords, which means there will just be color displayed. This engine does not support color mesh displays, only textured mesh!";
			ErrorMessage(str.str());
			continue;
		}

		if (!mesh->HasTangentsAndBitangents()) {
			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			str << " \n Problem: Tangents were not created. No known fix";
			ErrorMessage(str.str());
			continue;
		}

		// normals and tangents were created.. use those
		for (unsigned int x = 0; x < mesh->mNumVertices; ++x) {
			Model::Vertex v;
			v.position = XMFLOAT3(mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z);

			v.textureCoords = XMFLOAT2(mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y);

			v.normal = XMFLOAT3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z);

			v.tangent = XMFLOAT3(mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z);

			data.vertices[x] = v;
		}

		aiString path;

		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


		if (aiGetMaterialString(material, AI_MATKEY_TEXTURE_DIFFUSE(0), &path) == AI_SUCCESS) {

			/*aiString aiStr;
			stringstream str;
			material->Get(AI_MATKEY_NAME, aiStr);
			str << "AI_MATKEY_NAME: " << aiStr.data;
			str << "\n PATH: " << path.data;
			str << "\n modelname: " << modelData->modelName;
			str << "\n filepath: " << modelData->filepath;
			Utils::ErrorMessage(str);*/


			size_t origsize = strlen(path.data) + 1;
			const size_t newsize = 100;
			size_t convertedChars = 0;
			wchar_t wcstring[newsize];
			mbstowcs_s(&convertedChars, wcstring, origsize, path.data, _TRUNCATE);
			wcscat_s(wcstring, L"");


			wstring out = wstring(model->filepath.begin(), model->filepath.end());
			out += wstring(wcstring);
			const wchar_t* texturepath = out.c_str();

			//MessageBox(NULL, texturepath, L"CHECK", MB_OK); // check if filenames are correct

			Texture diffuse = Texture();
			if (diffuse.initialize(device, texturepath)) {
				data.hasTexture = true;
				data.texture = diffuse;
			} else
				data.hasTexture = false;

		} else {

			data.hasTexture = false;
			//MessageBox(NULL, L"No diffuse texture found", L"WARNING", MB_OK);

			/*stringstream str;
			str << "aiTextureType_AMBIENT: " << material->GetTextureCount(aiTextureType_AMBIENT);
			str << " \n aiTextureType_DIFFUSE: " << material->GetTextureCount(aiTextureType_DIFFUSE);
			str << " \n aiTextureType_DISPLACEMENT: " << material->GetTextureCount(aiTextureType_DISPLACEMENT);
			str << " \n aiTextureType_EMISSIVE: " << material->GetTextureCount(aiTextureType_EMISSIVE);
			str << " \n aiTextureType_HEIGHT: " << material->GetTextureCount(aiTextureType_HEIGHT);
			str << " \n aiTextureType_LIGHTMAP: " << material->GetTextureCount(aiTextureType_LIGHTMAP);
			str << " \n aiTextureType_NORMALS: " << material->GetTextureCount(aiTextureType_NORMALS);
			str << " \n aiTextureType_OPACITY: " << material->GetTextureCount(aiTextureType_OPACITY);
			str << " \n aiTextureType_REFLECTION: " << material->GetTextureCount(aiTextureType_REFLECTION);
			str << " \n aiTextureType_SHININESS: " << material->GetTextureCount(aiTextureType_SHININESS);
			str << " \n aiTextureType_SPECULAR: " << material->GetTextureCount(aiTextureType_SPECULAR);
			str << " \n aiTextureType_UNKNOWN: " << material->GetTextureCount(aiTextureType_UNKNOWN);
			Utils::ErrorMessage(str);*/
		}


		if (!initializeBuffers(device, &data)) {
			MessageBox(NULL, L"Error initializing Buffers", L"Buffer", MB_OK);
			return false;
		}

		model->meshData[i] = data;

	}

	return true;
}



bool MeshLoader::initializeBuffers(ID3D11Device* device, Model::MeshData* meshData) {


	Model::Vertex* tempVerts;
	unsigned long* tempIndices;

	tempVerts = new Model::Vertex[meshData->numVerts];
	tempIndices = new unsigned long[meshData->numIndices];

	
	for (int j = 0; j < meshData->numVerts; ++j) {
		tempVerts[j] = meshData->vertices[j];
		tempIndices[j] = j;
	}

	/**** VERTEX BUFFER ****/
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;	// D3D11_USAGE_DYNAMIC allows buffer to be updated
	vertexBufferDesc.ByteWidth = sizeof(Model::Vertex) * meshData->numVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = tempVerts;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &meshData->vertexBuffer))) {
		MessageBox(NULL, L"Error creating Vertex Buffer", L"ERROR", MB_OK);
		delete tempVerts;
		delete tempIndices;
		return false;
	}

	/**** INDEX BUFFER ****/
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * meshData->numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = tempIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &meshData->indexBuffer))) {
		MessageBox(NULL, L"Error creating Index Buffer", L"ERROR", MB_OK);
		delete tempVerts;
		delete tempIndices;
		return false;
	}


	return true;
}


//bool MeshLoader::loadMesh(ID3D11Device* device, const string filename) {
	//
	//	Assimp::Importer assimp;
	//	const aiScene* scene = NULL;
	//
	//
	//	scene = assimp.ReadFile(filename,
	//		aiProcess_Triangulate // translate models made from non triangle polygons into triangle based meshes
	//		| aiProcess_GenSmoothNormals // generates vertex normals if model does not already contain them
	//		| aiProcess_FlipUVs // flips texture coordinates along the Y axis
	//		| aiProcess_CalcTangentSpace
	//		| aiProcess_PreTransformVertices
	//		| aiProcess_FixInfacingNormals
	//		| aiProcess_FindInvalidData
	//		| aiProcess_ValidateDataStructure
	//		//| aiProcess_ConvertToLeftHanded
	//		//| aiProcess_MakeLeftHanded
	//		//| aiProcess_JoinIdenticalVertices
	//		//| aiProcess_SortByPType
	//		//| aiProcessPreset_TargetRealtime_MaxQuality
	//		//| aiProcess_LimitBoneWeights
	//		//| aiProcess_OptimizeMeshes
	//		);
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
	//	size_t loc = filename.find_last_of('/');
	//
	//
	//	modelData = new ModelData;
	//	modelData->modelName = filename.substr(loc + 1);
	//	modelData->filepath = filename.substr(0, loc + 1);
	//	modelData->numMeshes = scene->mNumMeshes;
	//	modelData->meshData.resize(modelData->numMeshes);
	//
	//
	//	if (!initFromScene(device, scene)) {
	//		delete(modelData);
	//		return false;
	//	}
	//
	//
	//	//if (currentvertex == 0) {// this could happen, if so GET OUTOF HERE
	//	//	OUTPUT_DEBUG_MSG("Problem loading the mesh, there were no vertices loaded. Failed to load the mesh");
	//	//	return false;
	//	//}
	//
	//
	//	return true;
	//}

//bool MeshLoader::initializeStaticBuffers(ID3D11Device* device) {
//
//	Vertex* tempVerts;
//	unsigned long* tempIndices;
//
//
//	tempVerts = new Vertex[modelData->numVerts];
//	tempIndices = new unsigned long[modelData->numIndices];
//
//	unsigned int currentVert = 0;
//
//	//for (int i = 0; i < modelData->numVerts; ++i) {
//	for (int i = 0; i < modelData->numMeshes; ++i) {
//
//		MeshData* mesh = &modelData->meshData[i];
//
//		for (int j = 0; j < mesh->numVerts; ++j) {
//			tempVerts[currentVert] = mesh->vertices[j];
//			tempIndices[currentVert] = currentVert;
//			++currentVert;
//		}
//	}
//
//
//	/**** STATIC VERTEX BUFFER ****/
//	D3D11_BUFFER_DESC vertexBufferDesc;
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(Vertex) * modelData->numVerts;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	D3D11_SUBRESOURCE_DATA vertexData;
//	vertexData.pSysMem = tempVerts;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &modelData->vertexStaticBuffer))) {
//		return false;
//	}
//
//	/**** STATIC INDEX BUFFER ****/
//	D3D11_BUFFER_DESC indexBufferDesc;
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * modelData->numIndices;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	D3D11_SUBRESOURCE_DATA indexData;
//	indexData.pSysMem = tempIndices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &modelData->indexStaticBuffer))) {
//		return false;
//	}
//
//	delete[] tempVerts;
//	delete[] tempIndices;
//
//	return true;
//}


//void MeshLoader::render(ID3D11DeviceContext* deviceContext) {
//
//	unsigned int stride = sizeof(Vertex);
//	unsigned int offset = 0;
//
//	for (int i = 0; i < modelData->numMeshes; ++i) {
//		deviceContext->IASetVertexBuffers(0, 1, &modelData->meshData[i].vertexBuffer, &stride, &offset);
//
//		// need the input layout in here I think. Anyway, it has to be called before finishing with IA.
//		deviceContext->IASetInputLayout(layout);
//
//		deviceContext->IASetIndexBuffer(modelData->meshData[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		deviceContext->DrawIndexed(modelData->meshData[i].numIndices, 0, 0);
//	}
//
//}


//void MeshLoader::renderStatic(ID3D11DeviceContext* deviceContext) {
//
//	unsigned int stride;
//	unsigned int offset;
//
//	stride = sizeof(Vertex);
//	offset = 0;
//
//	deviceContext->IASetVertexBuffers(0, 1, &modelData->vertexStaticBuffer, &stride, &offset);
//	deviceContext->IASetIndexBuffer(modelData->indexStaticBuffer, DXGI_FORMAT_R32_UINT, 0);
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//}



//int MeshLoader::getIndexCount() {
//
//	return modelData->numIndices;
//
//}

