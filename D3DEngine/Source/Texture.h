#pragma once

#include "D3DEnginePCH.h"
#include "Utils.h"

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

using namespace DirectX;


class Texture {
private:

	struct TargaHeader {
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:

	Texture();
	~Texture();

	/** Initialize a texture from non-DDS files. */
	bool initialize(ID3D11Device* device, const wchar_t* filepath);
	/** Initialize a texture from .dds file */
	bool initializeFromDDS(ID3D11Device* device, WCHAR* filename);
	/** Initialize a texture from .tga file */
	bool initializeFromTGA(ID3D11Device*, ID3D11DeviceContext*, char*);
	void shutdown();

	ID3D11ShaderResourceView* getTexture();



private:
	bool loadTarga(char*, int&, int&);


	unsigned char* targaData = 0;
	ID3D11Texture2D* texture2d = 0; // used for TGA files
	ID3D11Resource* texture = 0;
	ID3D11ShaderResourceView* textureView = 0;

	wchar_t textureName[50];
};

