#include "TextFactory.h"



TextFactory::TextFactory(ID3D11Device *dev, ID3D11DeviceContext *devCon) {

	device = dev;
	deviceContext = devCon;

	if (FAILED(FW1CreateFactory(FW1_VERSION, &pFW1Factory))) {
		MessageBox(NULL, L"Unable to create FontWrapperFactory", L"TextFactory ERROR", MB_OK);
		return;
	}

	if (FAILED(pFW1Factory->CreateFontWrapper(device, L"Arial", &pFontWrapper))) {
		MessageBox(NULL, L"Unable to create FontWrapper", L"TextFactory ERROR", MB_OK);
		return;
	}

	if (FAILED(pFontWrapper->GetDWriteFactory(&pDWriteFactory))) {
		MessageBox(NULL, L"Unable to create WriteFactory", L"TextFactory ERROR", MB_OK);
		return;
	}

	// Create the default DirectWrite text format to base layouts on
	if (FAILED(pDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"",
		&pTextFormat
		))) {

		MessageBox(NULL, L"Unable to create TextFormat", L"TextFactory ERROR", MB_OK);
		return;
	}


	// Create a text layout for a string
	const WCHAR str1[] = L"ANCIENT GREEK MYTHOLOGY";
	if (FAILED(pDWriteFactory->CreateTextLayout(
		str1,
		sizeof(str1) / sizeof(str1[0]),
		pTextFormat,
		0.0f,
		0.0f,
		&pTextLayout1
		))) {

		MessageBox(NULL, L"Unable to create TextLayout", L"TextFactory ERROR", MB_OK);
		return;
	}

	// No word wrapping
	if (FAILED(pTextLayout1->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP))) {
		MessageBox(NULL, L"Unable to SetWordWrapping", L"TextFactory ERROR", MB_OK);
		return;
	}
	if (FAILED(pTextLayout1->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER))) {
		MessageBox(NULL, L"Unable to SetTextAlignment", L"TextFactory ERROR", MB_OK);
		return;
	}

	// Set up typography features
	IDWriteTypography *pTypography;
	pDWriteFactory->CreateTypography(&pTypography);
	DWRITE_FONT_FEATURE fontFeature;
	fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_STYLISTIC_ALTERNATES;
	fontFeature.parameter = 1;
	pTypography->AddFontFeature(fontFeature);

	// Apply the typography features to select ranges in the string
	DWRITE_TEXT_RANGE textRange;

	textRange.startPosition = 0;
	textRange.length = 1;
	pTextLayout1->SetTypography(pTypography, textRange);
	textRange.startPosition = 10;
	textRange.length = 2;
	pTextLayout1->SetTypography(pTypography, textRange);
	textRange.startPosition = 18;
	textRange.length = 1;
	pTextLayout1->SetTypography(pTypography, textRange);

	pTypography->Release();

	// Set the font and size
	textRange.startPosition = 0;
	textRange.length = sizeof(str1) / sizeof(str1[0]);
	pTextLayout1->SetFontSize(64.0f, textRange);
	pTextLayout1->SetFontFamilyName(L"Pericles", textRange);


	// Create another text layout
	const WCHAR str2[] = L"A B C D E F G H I J K L M N\nA B C D E F G H I J K L M N";
	pDWriteFactory->CreateTextLayout(
		str2,
		sizeof(str2) / sizeof(str2[0]),
		pTextFormat,
		0.0f,
		0.0f,
		&pTextLayout2
		);
	pTextLayout2->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	pTextLayout2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	textRange.startPosition = 0;
	textRange.length = sizeof(str2) / sizeof(str2[0]);
	pTextLayout2->SetFontSize(48.0f, textRange);
	pTextLayout2->SetFontFamilyName(L"Pescadero", textRange);

	// Add font feature
	fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_SWASH;
	fontFeature.parameter = 1;
	pDWriteFactory->CreateTypography(&pTypography);
	pTypography->AddFontFeature(fontFeature);
	textRange.startPosition = 28;
	textRange.length = sizeof(str2) / sizeof(str2[0]) - 28;
	pTextLayout2->SetTypography(pTypography, textRange);
	pTypography->Release();

	// Set color overrides for select ranges
	IFW1ColorRGBA *pColorRed;
	pFW1Factory->CreateColor(0xff0000ff, &pColorRed);
	IFW1ColorRGBA *pColorGreen;
	pFW1Factory->CreateColor(0xff00ff00, &pColorGreen);

	textRange.startPosition = 2;
	textRange.length = 8;
	pTextLayout2->SetDrawingEffect(pColorRed, textRange);

	textRange.startPosition = 22;
	textRange.length = 15;
	pTextLayout2->SetDrawingEffect(pColorGreen, textRange);

	pColorRed->Release();
	pColorGreen->Release();


	// And a third text layout..
	const WCHAR codeStr[] = L"#include <iostream>\n\nint main() {\n    int num;\n    std::cin >> num;\n\n    for(int i=0; i < num; ++i)\n        std::cout << \"Hello World!\\n\";\n\n    return 0;\n}\n";
	pDWriteFactory->CreateTextLayout(
		codeStr,
		sizeof(codeStr) / sizeof(codeStr[0]),
		pTextFormat,
		0,
		0.0f,
		&pTextLayout3
		);
	pTextLayout3->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	textRange.startPosition = 0;
	textRange.length = sizeof(codeStr) / sizeof(codeStr[0]);
	pTextLayout3->SetFontSize(13.333333333f, textRange);
	pTextLayout3->SetFontFamilyName(L"Courier New", textRange);


	pFW1Factory->Release();
}


TextFactory::~TextFactory() {
}


void TextFactory::draw() {

	for (int i = 0; i < labels.size(); ++i) {
		pFontWrapper->DrawString(
			deviceContext,
			labels[i]->text.c_str(),// String
			labels[i]->fontSize,// Font size
			labels[i]->xPos,// X position
			labels[i]->yPos,// Y position
			labels[i]->fontColor,// Text color, 0xAaBbGgRr
			FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
			);
	}


	// Draw using text-layouts
	/*for (int i = 0; i < labels.size(); ++i) {

		pFontWrapper->DrawTextLayout(deviceContext, labels[i]->textLayout, labels[i]->xPos, labels[i]->yPos, labels[i]->fontColor, FW1_RESTORESTATE);
		}*/
}




TextLabel* TextFactory::createText(WCHAR* text, float xPos, float yPos, float fontSize) {

	TextLabel *label = new TextLabel();

	label->text = text;
	label->fontSize = fontSize;
	label->xPos = xPos;
	label->yPos = yPos;
	label->fontColor = 0xff0099ff;
	label->fontName = L"Ariel";


	labels.push_back(label);

	return label;
}


void TextFactory::editText(TextLabel* label, wstring str) {

	label->text = str;
	/*if (label->textLayout) {
		pDWriteFactory->CreateTextLayout(label->text.c_str(), static_cast<UINT32>(label->text.size()), pTextFormat, 0.0f, 0.0f, &label->textLayout);
		label->textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		label->textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		label->textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		DWRITE_TEXT_RANGE allText = {0, static_cast<UINT32>(label->text.size())};
		label->textLayout->SetFontSize(label->fontSize, allText);
		label->textLayout->SetFontFamilyName(label->fontName, allText);
	}*/
}


TextLabel* TextFactory::createTextLayout(WCHAR* text, float xPos, float yPos, float fontSize) {

	TextLabel *label = new TextLabel();

	label->text = text;
	label->fontSize = fontSize;
	label->xPos = xPos;
	label->yPos = yPos;
	label->fontColor = 0xff0099ff;


	label->fontName = L"Arial";

	pDWriteFactory->CreateTextLayout(label->text.c_str(), static_cast<UINT32>(label->text.size()), pTextFormat, 0.0f, 0.0f, &label->textLayout);
	label->textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	label->textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	label->textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	DWRITE_TEXT_RANGE allText = {0, static_cast<UINT32>(label->text.size())};
	label->textLayout->SetFontSize(label->fontSize, allText);
	label->textLayout->SetFontFamilyName(label->fontName, allText);

	labels.push_back(label);

	return label;
}


void TextFactory::createText(TextLabel *newLabel) {

	labels.push_back(newLabel);
}


void TextFactory::release() {


	if (labels.size() > 0)
		for (int i = labels.size() - 1; i >= 0; --i)
			delete labels[i];

	pTextLayout1->Release();
	pTextLayout2->Release();
	pTextLayout3->Release();

	pTextFormat->Release();
	pFontWrapper->Release();

	pDWriteFactory->Release();

}

