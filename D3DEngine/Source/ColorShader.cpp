#include "ColorShader.h"

ColorShader::ColorShader() {

}


ColorShader::~ColorShader() {

	ShutdownShader();
}


bool ColorShader::initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	if (!InitializeShader(device, hwnd, L"./source/shaders/VertexShader.hlsl", L"./source/shaders/PixelShader.hlsl")) {
		return false;
	}

	return true;
}


void ColorShader::shutdown() {

	ShutdownShader();

	return;
}



bool ColorShader::render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {


	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix)) {
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}


bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {

	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

		// Compile the vertex shader code.
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", Globals::VERTEX_SHADER_VERSION,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {

		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		} else {
			MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", Globals::PIXEL_SHADER_VERSION,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {

		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		} else {
			MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK);
		}

		return false;
	}


	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader))) {
		MessageBox(hwnd, L"Error creating Vertex Shader from buffer", L"ERROR", MB_OK);
		return false;
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader))) {

		MessageBox(hwnd, L"Error creating Pixel Shader from buffer", L"ERROR", MB_OK);
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout))) {
		MessageBox(hwnd, L"Error creating vertex input layout", L"ERROR", MB_OK);
		return false;
	}

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();


	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer))) {
		MessageBox(hwnd, L"Error creating constant buffer.", L"ERROR", MB_OK);
		return false;
	}

	return true;
}


void ColorShader::ShutdownShader() {
	// Release the matrix constant buffer.
	if (matrixBuffer) {
		matrixBuffer->Release();
	}

	// Release the layout.
	if (layout) {
		layout->Release();
	}

	// Release the pixel shader.
	if (pixelShader) {
		pixelShader->Release();
	}

	// Release the vertex shader.
	if (vertexShader) {
		vertexShader->Release();
	}

	return;
}


void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
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

	fout.close();

	errorMessage->Release();

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader. ***Mandatory in DX11***
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);


	if (FAILED(deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
		MessageBox(NULL, L"Error locking constant buffer.", L"ERROR", MB_OK);
		return false;
	}

	dataPtr = (MatrixBufferType*) mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);


	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	return true;
}


void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	
	deviceContext->IASetInputLayout(layout);
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
