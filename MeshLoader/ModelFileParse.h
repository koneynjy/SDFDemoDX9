#ifndef FILE_PARSE_H_
#define FILE_PARSE_H_
#include <stdio.h>
#include <string>
#include <vector>
#include "baseDefine.h"

#define MAX_PATH          260
#define TERRAIN			  "terrain"
#define BLENDTEX		  "blendTex"
#define BLENDTEX2		  "blendTex2.dds"
#define TEXINFOS		  "texInfos"
#define TERRAIN_DIR		  "\\dfw_wj\\"
#define CDATA_FILE        ".cdata"
#define CDATA_DIR         "dfw_wj\\"
#define FLAGS_DIR         "flags\\"
#define PRIMITIVES_FILE   ".primitives"


#define VERTEX			  "vertices"
#define INDEX			  "indices"

#define RENDERSET_ELEMENT	  "renderSet"
#define GEOMETRY_ELEMENT	  "geometry"
#define PRIMITIVEGROUP_ELEMENT	  "primitiveGroup"

#define DIFFUSETEXMAP	  "diffuseTexMap"
#define NORMALTEXMAP	  "normalTexMap"
#define SPECULARTEXMAP	  "specularTexMap"
#define DIFFUSEPOWER	"diffusePower"
#define SPECULARTEXS	"specularTexS"
#define SPECULARPOWER	"SpecularPower"

#define ALPHATEST "alphaTestEnable"
#define TWOSIDED "doubleSided"
#define ALPHAREF "alphaReference"



struct sec_info
{
	std::string   sec_file_name;
	unsigned long sec_size;
	unsigned long offset;
};

typedef std::vector<sec_info> sec_info_vector;

struct TexInfosBlock
{
	char		texPath[128];
	uchar		uvRepeatTime_;
	uchar		boxProjChannel_;
};

struct TerrainBlockHeader
{
	uint32		version_;
	uint32		heightMapWidth_;
	uint32		heightMapHeight_;
	float		spacing_;
	uint32		nTextures_;
	uint32		textureNameSize_;
	uint32		detailWidth_;
	uint32		detailHeight_;
	uchar		uvRepeatTimes_[4];
	uchar		boxProjChannel_[4];
	uint32		padding_[64 - 8 - sizeof(uchar)* 8 / sizeof(uint32)];
};




#endif //FILE_PARSE_H_