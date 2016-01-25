#include "BaseShader.h"

BaseShader::BaseShader() {
}

BaseShader::~BaseShader() {
}


void BaseShader::loadModel(Model* model) {

	models.push_back(model);
	
}


void BaseShader::render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantBuffers[]) {

	// set device context
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);


	deviceContext->IASetInputLayout(layout); // layout is one per shader??
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // this won't change??


	// for each model that uses this shader
	for (Model* model : models) {
		deviceContext->PSSetSamplers(0, 1, &samplerState); // is this per texture?
		model->render(deviceContext, constantBuffers);
	}
}


bool BaseShader::initializeShader(ID3D11Device* device, HWND hwnd,
	const ShaderDesc* shaderDesc) {

	ID3D10Blob* errorBlob;
	ID3D10Blob* vertexShaderBlob;
	ID3D10Blob* pixelShaderBlob;


	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// Compile the vertex shader code.
	if (FAILED(D3DCompileFromFile(shaderDesc->vsFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		shaderDesc->vsEP, Globals::VERTEX_SHADER_VERSION,
		flags, 0, &vertexShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, hwnd, shaderDesc->vsFile);
		} else {
			MessageBox(hwnd, shaderDesc->vsFile, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(D3DCompileFromFile(shaderDesc->psFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		shaderDesc->psEP, Globals::PIXEL_SHADER_VERSION,
		flags, 0, &pixelShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, hwnd, shaderDesc->psFile);
		} else {
			MessageBox(hwnd, shaderDesc->psFile, L"Missing Pixel Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(), NULL, &vertexShader))) {
		MessageBox(NULL, L"Error creating Vertex Shader", L"ERROR", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(), NULL, &pixelShader))) {
		MessageBox(NULL, L"Error creating Pixel Shader", L"ERROR", MB_OK);
		return false;
	}


	if (FAILED(initInputLayout(device, vertexShaderBlob))) {
		MessageBox(NULL, L"Error creating Input Layout Buffer", L"ERROR", MB_OK);
		return false;
	}

	vertexShaderBlob->Release();
	pixelShaderBlob->Release();


	if (FAILED(initMatrixBuffer(device))) {
		MessageBox(NULL, L"Error creating Constant (Matrix) Buffer", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(initSamplerState(device))) {
		MessageBox(NULL, L"Error creating Sampler State", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(initLightBuffer(device))) {
		return false;
	}


	return true;
}


HRESULT BaseShader::initInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob) {

	size_t i = 0;
	// needs to match the VertexTexture stucture in the Model/Mesh and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	polygonLayout[++i].SemanticName = "POSITION";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = 0;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	
	// Lighting layout
	polygonLayout[++i].SemanticName = "NORMAL";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;


	polygonLayout[++i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;

	// Tangent layout ... how useful is this?
	/*polygonLayout[++i].SemanticName = "TANGENT";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;*/


	/*polygonLayout[++i].SemanticName = "WORLDMATRIX";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;*/
	
	/*polygonLayout[++i].SemanticName = "INVERSETRANSPOSEWORLDMATRIX";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;*/


	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &layout))) {
		MessageBox(NULL, L"Error creating Input Layout", L"ERROR", MB_OK);
		return false;
	}

}


HRESULT BaseShader::initLightBuffer(ID3D11Device* device) {

	// light dynamic constant buffer description
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	/*D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(DiffuseLightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;*/

	//return device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	return NULL;
}


HRESULT BaseShader::initMatrixBuffer(ID3D11Device* device) {

	/*D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantMatrix);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;*/

	//return device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	return NULL;
}


HRESULT BaseShader::initSamplerState(ID3D11Device* device) {

	// Texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; // D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return device->CreateSamplerState(&samplerDesc, &samplerState);
}



void BaseShader::outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd,
	const WCHAR* shaderFilename) {

	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*) (errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}
	OutputDebugStringA(compileErrors);
	fout.close();
	errorMessage->Release();
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}



void BaseShader::release() {

	if (matrixBuffer)
		matrixBuffer->Release();
	if (samplerState)
		samplerState->Release();
	if (layout)
		layout->Release();
	if (pixelShader)
		pixelShader->Release();
	if (vertexShader)
		vertexShader->Release();
	if (lightBuffer)
		lightBuffer->Release();

	for each (Model* model in models) {
		delete model;
	}
}
