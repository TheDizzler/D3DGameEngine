#include "ShaderBase.h"



ShaderBase::ShaderBase() {
}


ShaderBase::~ShaderBase() {

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
}


bool ShaderBase::initialize(ID3D11Device *device, HWND hwnd, const WCHAR * vsFilename, const WCHAR * psFilename) {

	if (!initializeShader(device, hwnd, vsFilename, psFilename))
		return false;

	return true;
}


void ShaderBase::outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename) {

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
