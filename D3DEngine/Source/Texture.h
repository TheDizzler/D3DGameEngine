#pragma once

#include <d3d11.h>
#include <stdio.h>

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

	bool initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void shutdown();

	ID3D11ShaderResourceView* getTexture();



private:
	bool loadTarga(char*, int&, int&);


	unsigned char* targaData;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;
};

