#include "Mesh.h"



Mesh::Mesh() {
}


Mesh::~Mesh() {

	release();
}





bool Mesh::loadMesh(ID3D11Device* device, const string filename) {

	Assimp::Importer assimp;
	const aiScene* scene = NULL;


	scene = assimp.ReadFile(filename,
		aiProcess_Triangulate // translate models made from non triangle polygons into triangle based meshes
		| aiProcess_GenSmoothNormals // generates vertex normals if model does not already contain them
		| aiProcess_FlipUVs // flips texture coordinates along the Y axis
		| aiProcess_CalcTangentSpace
			/*| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
			| aiProcessPreset_TargetRealtime_MaxQuality
			| aiProcess_ConvertToLeftHanded*/);


	if (!scene) {
		char* error = (char*) assimp.GetErrorString();
		wchar_t wtext[256];
		mbstowcs(wtext, error, strlen(error) + 1);
		MessageBox(NULL, wtext, L"Assimp Error", MB_OK);
		return false;
	}

	size_t loc = filename.find_last_of('/');


	modelData = new ModelData;
	modelData->modelName = filename.substr(loc + 1);
	modelData->filepath = filename.substr(0, loc + 1);
	modelData->numMeshes = scene->mNumMeshes;
	modelData->meshData.resize(modelData->numMeshes);


	if (!initFromScene(device, scene)) {
		return false;
	}

	
	//if (currentvertex == 0) {// this could happen, if so GET OUTOF HERE
	//	OUTPUT_DEBUG_MSG("Problem loading the mesh, there were no vertices loaded. Failed to load the mesh");
	//	return false;
	//}

	if (!initializeBuffers(device)) {
		return false;
	}

	return true;
}



bool Mesh::initFromScene(ID3D11Device* device, const aiScene* scene) {


	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		modelData->numVerts += scene->mMeshes[i]->mNumVertices;
		modelData->numIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	int stride = (modelData->numVerts >= 65536 ? 4 : 2);


	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {

		const aiMesh* mesh = scene->mMeshes[i];

		MeshData* data = new MeshData;
		data->numVerts = mesh->mNumVertices;
		data->numIndices = mesh->mNumFaces * 3;
		data->vertices.resize(mesh->mNumVertices);

		if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {

			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			if (mesh->mPrimitiveTypes == aiPrimitiveType_LINE)
				str << " \n Problem: The mesh containes lines when it should only contain triangles";
			else
				str << " \n Problem: The mesh containes points when it should only contain triangles";

			ERRORMESSAGE(str);

			continue;
		}

		if (!mesh->HasTextureCoords(0)) {
			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			str << " \n Problem: The mesh containes no texcoords, which means there will just be color displayed. This engine does not support color mesh displays, only textured mesh!";
			ERRORMESSAGE(str);
			continue;
		}

		if (!mesh->HasTangentsAndBitangents()) {
			stringstream str;
			str << "There are errors with this submesh, named: " << mesh->mName.data;
			str << " \n Problem: Tangents were not created. No known fix";
			ERRORMESSAGE(str);
			continue;
		}

		// normals and tangents were created.. use those
		for (unsigned int x = 0; x < mesh->mNumVertices; ++x) {
			Vertex v;
			//tempverts[x + currentVertex].position =
			v.position =
				XMFLOAT3(mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z);

			//tempverts[x + currentVertex].texture =
			v.textureCoords
				= XMFLOAT2(mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y);

			//tempverts[x + currentVertex].normal =
			v.normal
				= XMFLOAT3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z);

			//tempverts[x + currentVertex].Tang =
			v.tangent
				= XMFLOAT3(mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z);

			data->vertices[x] = v;
		}

		//loadTextures(data, mesh);

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
			ERRORMESSAGE(str);*/


			size_t origsize = strlen(path.data) + 1;
			const size_t newsize = 100;
			size_t convertedChars = 0;
			wchar_t wcstring[newsize];
			mbstowcs_s(&convertedChars, wcstring, origsize, path.data, _TRUNCATE);
			wcscat_s(wcstring, L"");

			wstring out = wstring(modelData->filepath.begin(), modelData->filepath.end());
			out += wstring(wcstring);
			const wchar_t* texturepath = out.c_str();

			//MessageBox(NULL, texturepath, L"HU>", MB_OK);

			Texture* diffuse = new Texture();
			diffuse->initialize(device, texturepath);

			data->texture = diffuse;
		} else {

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
			ERRORMESSAGE(str);*/
		}



		modelData->meshData[i] = *data;


			//loadMaterials(mesh, load->mMaterials, batch, pathtomodel, Asset_Dir);
		//	batch->NumIndices = mesh->mNumFaces * 3;
		//	batch->StartIndex = static_cast<uint32_t>(currentindex);
		//	batch->NumVerts = mesh->mNumVertices;
		//	// make sure to increment the ref count for thesse so they are properly destroyed

		//	currentvertex += mesh->mNumVertices;
		//	currentindex += mesh->mNumFaces * 3;

		//	//For now, there will be a new shader for each material. I will create a shader cache where the graphics lib will cache the shaders and issue out already created shaders like it does with textures.
		//	Graphics::Shader_Macro macro1[] = {
		//		{"NORMALMAP", "1"},
		//		{NULL, NULL}
		//	};
		//	Graphics::Shader_Macro macro0[] = {
		//		{"NORMALMAP", "0"},
		//		{NULL, NULL}
		//	};

		//	Graphics::Shader_Macro* ptr = nullptr;
		//	if (batch->Has_NormalMap()) ptr = macro1;
		//	else ptr = macro0;

		//	batch->GetVS()->CompileShaderFromFile("Static_Mesh.fx", "VS", "vs_4_0", ptr);

		//	FormatDesc lay[] = {
		//		FormatDesc(),
		//		FormatDesc(TYPE_TEXCOORD, FORMAT_FLOAT, 2),
		//		FormatDesc(TYPE_NORMAL, FORMAT_FLOAT, 3),
		//		FormatDesc(TYPE_TANGENT, FORMAT_FLOAT, 3)
		//	};
		//	batch->GetVS()->CreateInputLayout(lay, 4);
		//	batch->GetPS()->CompileShaderFromFile("Static_Mesh.fx", "PS", "ps_4_0", ptr);

		//	Batches.push_back(batch);
	}

	return true;
}


void Mesh::loadTextures(MeshData* data, const aiMesh* aiMesh) {

	//const aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

}




bool Mesh::initializeBuffers(ID3D11Device* device) {

	Vertex* tempVerts;
	unsigned long* tempIndices;


	tempVerts = new Vertex[modelData->numVerts];
	tempIndices = new unsigned long[modelData->numIndices];

	unsigned int currentVert = 0;

	//for (int i = 0; i < modelData->numVerts; ++i) {
	for (int i = 0; i < modelData->numMeshes; ++i) {

		MeshData* mesh = &modelData->meshData[i];

		for (int j = 0; j < mesh->numVerts; ++j) {
			tempVerts[currentVert] = mesh->vertices[j];
			tempIndices[currentVert] = currentVert;
			++currentVert;
		}
	}


	/**** STATIC VERTEX BUFFER ****/
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * modelData->numVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = tempVerts;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &modelData->vertexBuffer))) {
		return false;
	}

	/**** STATIC INDEX BUFFER ****/
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * modelData->numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = tempIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &modelData->indexBuffer))) {
		return false;
	}

	delete[] tempVerts;
	delete[] tempIndices;

	return true;
}



void Mesh::render(ID3D11DeviceContext* deviceContext) {

	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &modelData->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(modelData->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


}

//bool Mesh::loadTest(const string fileName) {
//	Assimp::Importer imp;
//	const aiScene* scene = NULL;
//	const aiMesh* mesh = NULL;
//
//	scene = imp.ReadFile(fileName, aiProcess_Triangulate);
//	if (!scene) {
//		MessageBox(NULL, L"Error read file", L"Error", MB_OK);
//		return false;
//	}
//
//	numVerts = numIndices = 0;
//
//	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
//		numVerts += scene->mMeshes[i]->mNumVertices;
//		numIndices += scene->mMeshes[i]->mNumFaces * 3;
//	}
//
//	stringstream sstest;
//	sstest << "numVerts: " << numVerts << " numIndices: " << numIndices;
//	string test = sstest.str();
//	wstring wtest;
//	for (int i = 0; i < test.size(); ++i)
//		wtest += wchar_t(test[i]);
//	MessageBox(NULL, wtest.c_str(), L"Seems ok here", MB_OK);
//
//	mesh = scene->mMeshes[0];
//	if (!mesh) {
//		MessageBox(NULL, L"Failed to find meshes", L"Error", MB_OK);
//		return false;
//	}
//
//
//
//	return true;
//}


int Mesh::getIndexCount() {

	return modelData->numIndices;

}

void Mesh::ERRORMESSAGE(stringstream& msg) {

	string str = msg.str();
	wstring wstr;
	for (int i = 0; i < str.size(); ++i)
		wstr += wchar_t(str[i]);
	MessageBox(NULL, wstr.c_str(), L"This is an Error Msg", MB_OK);
}


void Mesh::release() {

	modelData->release();
	delete modelData;
}
