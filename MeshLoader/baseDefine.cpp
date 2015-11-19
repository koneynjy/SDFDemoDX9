#include "baseDefine.h"

void get_res_dir(std::string& str)
{
	//str = RES_DIR;
	str = MY_DIR;
	return;//now use it;
}

void get_program_dir(std::string& str)
{
	char charArray[MAX_PATH];
	ZeroMemory(charArray, MAX_PATH);
	::GetModuleFileNameA(NULL, charArray, MAX_PATH);


	size_t length = strlen(charArray);
	for (size_t i = length - 1; i >= 0; i--)
	{
		if (charArray[i] != '\\')
		{
			charArray[i] = 0;
		}
		else
			break;
	}

	str = charArray;
}