#pragma once

#include <d3d11.h>
#include <stdio.h>

#include "DDSTextureLoader.h"

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

	/** Initialize a texture from .dds file */
	bool initialize(ID3D11Device* device, WCHAR* filename);
	/** Initialize a texture from .tga file */
	bool initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void shutdown();

	ID3D11ShaderResourceView* getTexture();



private:
	bool loadTarga(char*, int&, int&);


	unsigned char* targaData = 0;
	ID3D11Texture2D* texture = 0;
	ID3D11ShaderResourceView* textureView;
};

