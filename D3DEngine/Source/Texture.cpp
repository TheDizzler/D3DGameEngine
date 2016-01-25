#include "Texture.h"


Texture::Texture() {

}



Texture::~Texture() {
}


/** Initialize a texture from non-DDS files. */
bool Texture::initialize(ID3D11Device* device, const wchar_t* filename) {

	if (reportError(CreateWICTextureFromFile(device, filename, &texture, &textureView))) {
		stringstream str;
		str << "Failed CreateShaderResourceViewFromFile, file name: " << filename;
		ErrorMessage(str.str());
		//MessageBox(NULL, L"Failed CreateShaderResourceViewFromFile", L"ERROR", MB_OK);
		return false;
	}

	wcscpy(textureName, filename);

	return true;
}


/** Initialize a texture from .dds file */
bool Texture::initializeFromDDS(ID3D11Device* device, WCHAR* filename) {


	if (reportError(CreateDDSTextureFromFile(device, filename, NULL, &textureView))) {
		MessageBox(NULL, L"Failed to D3DX11CreateShaderResourceViewFromFile", L"ERROR", MB_OK);
		return false;
	}

	return true;
}

/** Initialize a texture from .tga file */
bool Texture::initializeFromTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;


	// Load the targa image data into memory.
	result = loadTarga(filename, height, width);
	if (!result) {
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &texture2d);
	if (reportError(hResult)) {
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(texture2d, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(texture2d, &srvDesc, &textureView);
	if (reportError(hResult)) {
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(textureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;
	targaData = 0;

	return true;
}


void Texture::shutdown() {


	if (textureView) {
		textureView->Release();
	}

	if (texture2d) {
		texture2d->Release();
	}

	if (texture)
		texture->Release();

	if (targaData) {
		delete[] targaData;

	}

}


ID3D11ShaderResourceView* Texture::getTexture() {
	//if (textureView)
	//MessageBox(NULL, textureName, L"Check Getting Texture", MB_OK); // check if filenames are correct
	return textureView;
/*else
	return NULL;*/
}


bool Texture::loadTarga(char* filename, int& height, int& width) {
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) {
		return false;
	}

	// Read in the file header.
	count = (unsigned int) fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Get the important information from the header.
	height = (int) targaFileHeader.height;
	width = (int) targaFileHeader.width;
	bpp = (int) targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32) {
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage) {
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int) fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	// Allocate memory for the targa destination data.
	targaData = new unsigned char[imageSize];
	if (!targaData) {
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			targaData[index + 0] = targaImage[k + 2];  // Red.
			targaData[index + 1] = targaImage[k + 1];  // Green.
			targaData[index + 2] = targaImage[k + 0];  // Blue
			targaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;

	return true;
}