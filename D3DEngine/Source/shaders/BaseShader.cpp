#include "BaseShader.h"

BaseShader::BaseShader() {
}

BaseShader::~BaseShader() {
}


void BaseShader::loadModel(Model* model) {

	models.push_back(*model); // making an attempt at keeping models in contiguous memory. Pointless?
}


void BaseShader::render(ID3D11DeviceContext* deviceContext) {

	// set device context
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);


	deviceContext->IASetInputLayout(layout); // let is one per shader??
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // this won't change??

	// for each model that uses this shader
	for (Model model : models) {
		model.render(deviceContext);
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
	if (FAILED(D3DCompileFromFile(&shaderDesc->vsFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		shaderDesc->vsEntryPoint, Globals::VERTEX_SHADER_VERSION,
		flags, 0, &vertexShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, hwnd, &shaderDesc->vsFilename);
		} else {
			MessageBox(hwnd, &shaderDesc->vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(D3DCompileFromFile(&shaderDesc->psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		shaderDesc->psEntryPoint, Globals::PIXEL_SHADER_VERSION,
		flags, 0, &pixelShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, hwnd, &shaderDesc->psFilename);
		} else {
			MessageBox(hwnd, &shaderDesc->psFilename, L"Missing Pixel Shader File", MB_OK);
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

	safeRelease(vertexShaderBlob);
	safeRelease(pixelShaderBlob);


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






HRESULT BaseShader::initInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob) {

	// needs to match the VertexTexture stucture in the Model/Mesh and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
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
	if (sampleState)
		sampleState->Release();
	if (layout)
		layout->Release();
	if (pixelShader)
		pixelShader->Release();
	if (vertexShader)
		vertexShader->Release();
	if (lightBuffer)
		lightBuffer->Release();

	for (Model model : models) {
		delete model;
	}
}
