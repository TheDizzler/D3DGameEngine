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


/** Initialize with a texture. */
bool Model::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename) {


	if (!initializeTextureBuffers(device)) {
		MessageBox(NULL, L"Error intializing Buffers", L"ERROR", MB_OK);
		return false;
	}
	if (!loadTexture(device, deviceContext, textureFilename)) {
		MessageBox(NULL, L"Error loading Texture.", L"ERROR", MB_OK);
		return false;
	}

	hasTexture = true;

	return true;
}


void Model::shutdown() {
	if (hasTexture)
		releaseTexture();
	shutdownBuffers();
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


bool Model::initializeTextureBuffers(ID3D11Device* device) {

	VertexTexture* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the number of vertices in the vertex array.
	vertexCount = 3;

	// Set the number of indices in the index array.
	indexCount = 3;

	// Create the vertex array.
	vertices = new VertexTexture[vertexCount];
	if (!vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices) {
		return false;
	}

		// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

					 // Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTexture) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


/** Creates Index and Vertex Buffers for Texture-less models. */
bool Model::initializeBuffers(ID3D11Device *device) {

	VertexColor* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	/**** TEMP ***/
	// Set the number of vertices in the vertex array.
	vertexCount = 3;

	// Set the number of indices in the index array.
	indexCount = 3;

	// Create the vertex array.
	vertices = new VertexColor[vertexCount];
	if (!vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices) {
		return false;
	}

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.



					 // Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexColor) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
		return false;
	}

	delete[] vertices;
	delete[] indices;

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
		stride = sizeof(VertexTexture);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool Model::loadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {


	texture = new Texture;

	// Initialize the texture object.
	if (!texture->initialize(device, deviceContext, filename)) {
		MessageBox(NULL, L"Error trying to initialize texture", L"Error", MB_OK);
		return false;
	}

	return true;
}


void Model::releaseTexture() {

		texture->shutdown();
		delete texture;
}


void Model::shutdownBuffers() {

	if (indexBuffer) {
		indexBuffer->Release();
	}

	if (vertexBuffer) {
		vertexBuffer->Release();
	}
}
