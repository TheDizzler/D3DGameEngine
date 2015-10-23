/* Helper class to take advantage of FW1FontWrapper capabilities.
	Written by Tristan Gillon. */
#pragma once

//#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "FW1FontWrapper.lib")

#include <D3D11.h>
#include <comdef.h>
#include <vector>
#include "..\FW1FontWrapper\FW1FontWrapper.h"


using namespace std;

//const wchar_t* fonts[] = {L"Arial", L"Courier New", L"MS Sans Serif", L"Times New Roman"};


class TextLabel {
public:
	wstring text;
	float xPos;
	float yPos;
	float fontSize;
	const WCHAR *fontName;
	IDWriteTextLayout *textLayout;

	UINT32 fontColor = 0xff0099ff;

	TextLabel() {

	}

	/*TextLabel(WCHAR* t, float x, float y, float fSize, UINT32 fCol):
		text(t), xPos(x), yPos(y), fontSize(fSize), fontColor(fCol) {}*/

};


class TextFactory {
public:


	TextFactory(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	~TextFactory();

	void draw();

	TextLabel* createText(WCHAR* text, float xPos, float yPos, float fontSize);
	void createText(TextLabel *newLabel);
	TextLabel* createTextLayout(WCHAR* text, float xPos, float yPos, float fontSize);
	void editText(TextLabel* label, wstring str);

	void release();
private:

	vector<TextLabel*> labels;

	IFW1Factory *pFW1Factory;
	IDWriteFactory *pDWriteFactory;
	IFW1FontWrapper *pFontWrapper;

	IDWriteTextLayout *pTextLayout1, *pTextLayout2, *pTextLayout3;
	IDWriteTextFormat *pTextFormat;

	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
};

