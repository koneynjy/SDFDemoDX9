#include "assert.h"
#include <iostream>
#include "TGALoader.h"

#pragma optimize("", off)
/** 构造函数 */
CTGALoader::CTGALoader()
{
	/** 设置为默认值 */
	image = 0;
	ID = -1;
	imageWidth = 0;
	imageHeight = 0;
}
/** 析构函数 */
CTGALoader::~CTGALoader()
{
	FreeImage();           /**< 释放内存 */
}
/** 载入TGA文件 */
bool CTGALoader::LoadTGA(const string& filename)
{
	if (filename.length() < 5) return false;
	string format = filename.substr(filename.length() - 4, 4);
	if (format != ".tga" && format != ".TGA") return false;
	FILE *pfile;

	/** 打开文件 */
	pfile = fopen(filename.c_str(), "rb");
	//pfile = fopen("D:/logo2.tga", "rb");
	if (!pfile)			   
		return false;


	fseek(pfile, 2 * sizeof(uchar), SEEK_CUR);
	fread(&ctType, sizeof(uchar), 1, pfile);

	//width
	fseek(pfile, 4 * sizeof(uint16)+sizeof(uchar), SEEK_CUR);
	fread(&imageWidth, sizeof(uint16), 1, pfile);

	//height
	fread(&imageHeight, sizeof(uint16), 1, pfile);
	fread(&depth, sizeof(uchar), 1, pfile);
	fseek(pfile, sizeof(uchar), SEEK_CUR);

	//color indexed
	if (ctType == 1)
	{
		fclose(pfile);
		assert(0, "文件类型错误!");
		return false;
	}

	if (ctType != 2 && ctType != 3)
	{
		fclose(pfile);
		assert(0, "文件类型错误!");
		return false;
	}

	byteCount = depth >> 3;
	size = imageWidth * imageHeight * byteCount;

	image = new uchar[size];
	fread(image, size, 1, pfile);

	if (image == NULL)
	{
		fclose(pfile);
		return false;
	}

	if (ferror(pfile))
	{
		fclose(pfile);
		return false;
	}

	if (byteCount >= 3) //RGB RGBA
	{
		for (int i = 0; i < size; i += byteCount)     //convert from bgr to rgb
		{
			uchar tmp = image[i];
			image[i] = image[i + 2];
			image[i + 2] = tmp;
		}
	}

	fclose(pfile);

	return true;
}

void CTGALoader::FreeImage()
{
	/** 释放内存 */
	if (image)
	{
		delete[] image;
		image = 0;
	}
}