#include "Utils.h"


void Utils::ErrorMessage(stringstream& msg) {

	string str = msg.str();
	wstring wstr;
	for (int i = 0; i < str.size(); ++i)
		wstr += wchar_t(str[i]);
	MessageBox(NULL, wstr.c_str(), L"This is an Error Msg", MB_OK);
}