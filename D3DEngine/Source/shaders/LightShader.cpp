#include "LightShader.h"



LightShader::LightShader() {
}


LightShader::~LightShader() {
}


void LightShader::release() {
	if (lightBuffer)
		lightBuffer->Release();
}


bool LightShader::initializeShader(ID3D11Device * device, HWND hwnd, const WCHAR * vsFilename, const WCHAR * psFilename) {

	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;



	// Compile the vertex shader code.
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "DiffuseLightVertexShader", Globals::VERTEX_SHADER_VERSION,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
		if (errorMessage) {
			outputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		} else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "DiffuseLightPixelShader", Globals::PIXEL_SHADER_VERSION,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
		if (errorMessage) {
			outputShaderErrorMessage(errorMessage, hwnd, psFilename);
		} else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader))) {
		MessageBox(NULL, L"Error creating Vertex Shader", L"ERROR", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader))) {
		MessageBox(NULL, L"Error creating Pixel Shader", L"ERROR", MB_OK);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	// needs to match the VertexTexture stucture in the Model/Mesh and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Lighting layout
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Tangent layout
	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;


	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout))) {
		MessageBox(NULL, L"Error creating Input Layout", L"ERROR", MB_OK);
		return false;
	}

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();


	if (FAILED(initMatrixBuffer(device))) {
		MessageBox(NULL, L"Error creating Constant (Matrix) Buffer", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(initSamplerState(device))) {
		MessageBox(NULL, L"Error creating Sampler Shader", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(initLightBuffer(device))) {
		return false;
	}

	return true;
}


HRESULT LightShader::initLightBuffer(ID3D11Device* device) {

	// light dynamic constant buffer description
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(DiffuseLightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	return device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}


HRESULT LightShader::initMatrixBuffer(ID3D11Device* device) {

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantMatrix);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	return device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
}


HRESULT LightShader::initSamplerState(ID3D11Device* device) {

	// Texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return device->CreateSamplerState(&samplerDesc, &sampleState);
}


bool LightShader::render(ID3D11DeviceContext *deviceContext, Mesh* mesh, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix, DiffuseLight* light) {

	if (!setShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, mesh,
		light->direction, light->diffuseColor, light->ambientColor)) {
		return false;
	}

	renderShader(deviceContext, mesh->getIndexCount());
	return true;
}




bool LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Mesh* mesh,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT4 ambientColor) {


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantMatrix* dataPtr;
	unsigned int bufferNumber =0;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	
	for (int i = 0; i < mesh->modelData->numMeshes; ++i) {
		if (FAILED(deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			return false;
		}

		dataPtr = (ConstantMatrix*) mappedResource.pData;

		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		deviceContext->Unmap(matrixBuffer, 0);
		bufferNumber = 0;


		Mesh::MeshData data = mesh->modelData->meshData[i];
		ID3D11ShaderResourceView* texture;

		if (data.texture)
			texture = data.texture->getTexture();
		else
			texture = NULL;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
		deviceContext->PSSetShaderResources(0, 1, &texture);

		/***** Light Constant Buffer *****/
		DiffuseLightBuffer* dataPtr2;
		// Must lock light constant buffer so can write to it
		if (FAILED(deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			return false;
		}

		dataPtr2 = (DiffuseLightBuffer*) mappedResource.pData;
		dataPtr2->ambientColor = ambientColor;
		dataPtr2->diffuseColor = diffuseColor;
		dataPtr2->lightDirection = lightDirection;
		dataPtr2->padding = 0.0f;

		deviceContext->Unmap(lightBuffer, 0);	// Unlock the constant buffer.
		bufferNumber = 0;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);
		//renderShader(deviceContext, data.numIndices);
	}
	return true;
}




bool LightShader::render(ID3D11DeviceContext *deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {

	if (!setShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,
		lightDirection, diffuseColor)) {
		return false;
	}

	renderShader(deviceContext, indexCount);
	return true;
}


bool LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantMatrix* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	if (FAILED(deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
		return false;
	}

	dataPtr = (ConstantMatrix*) mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(matrixBuffer, 0);
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	/***** Light Constant Buffer *****/
	DiffuseLightBuffer* dataPtr2;
	// Must lock light constant buffer so can write to it
	if (FAILED(deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
		return false;
	}

	dataPtr2 = (DiffuseLightBuffer*) mappedResource.pData;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	deviceContext->Unmap(lightBuffer, 0);	// Unlock the constant buffer.
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	return true;
}


void LightShader::renderShader(ID3D11DeviceContext* deviceContext, int indexCount) {

	deviceContext->IASetInputLayout(layout);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}

