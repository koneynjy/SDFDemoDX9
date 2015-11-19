#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "Config.h"

struct FTexture
{
	uint16 width;
	uint16 height;
	uint8 byteCount;
	uint8 *data;
	FTexture() :data(0),width(0), height(0), byteCount(0){}

	~FTexture()
	{
		if (width == 0)
			delete data;
	}
};

struct FMaterial
{
	bool twoSided;
	bool alphaTest;
	uint8 alphaRef;
	FTexture diffuse;
	FMaterial() :twoSided(false), alphaTest(false), alphaRef(0){};
	FORCEINLINE int SampleAlphaTest(int offset)
	{
		if (diffuse.data)
		{
			return -((diffuse.data + offset)[diffuse.byteCount - 1] >= alphaRef);
		}
		return 0;
	}
};

#endif // !_MATERIAL_H
