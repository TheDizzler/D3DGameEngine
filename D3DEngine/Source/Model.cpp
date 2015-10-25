#include "Model.h"



Model::Model() {
}


Model::~Model() {
}

/** Initialize without a texture. */
bool Model::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {


	if (!initializeBuffers(device)) {
		MessageBox(NULL, L"Error intializing Buffers", L"ERROR", MB_OK);
		return false;
	}

	hasTexture = false;

	return true;

}


bool Model::initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename) {

	if (!loadModel(modelFilename)) {
		MessageBox(NULL, L"Could not load model data.", L"ERROR", MB_OK);
		return false;
	}

	if (!initializeLightBuffers(device)) {
		MessageBox(NULL, L"Error intializing Buffers", L"ERROR", MB_OK);
		return false;
	}

	if (!loadTexture(device, textureFilename)) {
		MessageBox(NULL, L"Error loading Texture.", L"ERROR", MB_OK);
		return false;
	}

	hasTexture = true;

	return true;
}


/** Initialize with a texture. */
bool Model::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename) {


	//if (!initializeTextureBuffers(device)) {
	if (!initializeLightBuffers(device)) {
		MessageBox(NULL, L"Error intializing Buffers", L"ERROR", MB_OK);
		return false;
	}

	if (!loadTGATexture(device, deviceContext, textureFilename)) {
		MessageBox(NULL, L"Error loading Texture.", L"ERROR", MB_OK);
		return false;
	}

	hasTexture = true;

	return true;
}


void Model::render(ID3D11DeviceContext *deviceContext) {

	renderBuffers(deviceContext);

}

int Model::getIndexCount() {
	return indexCount;
}

ID3D11ShaderResourceView* Model::getTexture() {
	if (hasTexture)
		return texture->getTexture();
	else
		return NULL;
}


bool Model::initializeLightBuffers(ID3D11Device* device) {

	VertexLight* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertices = new VertexLight[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < vertexCount; ++i) {

		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = XMFLOAT2(model[i].tu, model[i].tv);
		vertices[i].normal = XMFLOAT3(model[i].nx, model[i].ny, model[i].nz);

		indices[i] = i;
	}


	/**** STATIC VERTEX BUFFER ****/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexLight) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
		return false;
	}

	/**** STATIC INDEX BUFFER ****/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
		return false;
	}

	delete[] vertices;
	delete[] indices;

	return true;
}


bool Model::initializeTextureBuffers(ID3D11Device* device) {

	VertexTexture* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	

	vertices = new VertexTexture[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < vertexCount; ++i) {
		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = XMFLOAT2(model[i].tu, model[i].tv);

		indices[i] = i;
	}


	/**** STATIC VERTEX BUFFER ****/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTexture) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
		return false;
	}

	/**** STATIC INDEX BUFFER ****/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
		return false;
	}

	delete[] vertices;
	delete[] indices;

	return true;
}


/** Creates Index and Vertex Buffers for Texture-less models. */
bool Model::initializeBuffers(ID3D11Device *device) {

	VertexColor* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;


	// Create the vertex array.
	vertices = new VertexColor[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < vertexCount; ++i) {
		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].color = XMFLOAT4(model[i].nx, model[i].ny, model[i].nz, 0.5f); // cause why the hell not
	}


	/**** STATIC VERTEX BUFFER ****/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexColor) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
		return false;
	}

	/**** STATIC INDEX BUFFER ****/
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
		return false;
	}

	delete[] vertices;
	delete[] indices;

	return true;
}


bool Model::loadModel(char* filename) {

	ifstream fin;
	char input;
	int i;

	fin.open(filename);

	if (fin.fail()) {
		return false;
	}

	// Find Vertex Count
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	fin >> vertexCount;
	indexCount = vertexCount;

	model = new ModelType[vertexCount];

	// start reading vertex data
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for (i = 0; i<vertexCount; i++) {
		fin >> model[i].x >> model[i].y >> model[i].z;
		fin >> model[i].tu >> model[i].tv;
		fin >> model[i].nx >> model[i].ny >> model[i].nz;
	}

	fin.close();

	return true;
}


// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
void Model::renderBuffers(ID3D11DeviceContext *deviceContext) {

	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	if (!hasTexture)
		stride = sizeof(VertexColor);
	else
		//stride = sizeof(VertexTexture);
		stride = sizeof(VertexLight);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool Model::loadTexture(ID3D11Device* device, WCHAR* filename) {


	texture = new Texture;

	// Initialize the texture object.
	if (!texture->initialize(device, filename)) {
		MessageBox(NULL, L"Error trying to initialize texture", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Model::loadTGATexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {


	texture = new Texture;

	// Initialize the texture object.
	if (!texture->initialize(device, deviceContext, filename)) {
		MessageBox(NULL, L"Error trying to initialize texture", L"Error", MB_OK);
		return false;
	}

	return true;
}



void Model::shutdown() {

	if (hasTexture) {
		texture->shutdown();
		delete texture;
	}

	if (indexBuffer) {
		indexBuffer->Release();
	}

	if (vertexBuffer) {
		vertexBuffer->Release();
	}

	if (model)
		delete[] model;
}