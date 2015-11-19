#ifndef BASE_DEFINE_H
#define BASE_DEFINE_H
#include <string>
#include <map>
#include <vector>
#include "windows.h"
#include "xnamath.h"

typedef unsigned int  uint32;
typedef unsigned char uchar;
typedef unsigned short uint16;

typedef std::map<std::string, std::string> FILE_MAP;
typedef std::vector<std::string> STRING_VECTOR;

#define RES_DIR			  "i:\\bw\\tw2\\res\\"
#define MY_DIR            ""

// Position, Normal, UV, Tangent, Binormal
 struct VertexXYZNUVTB
{
	XMFLOAT3		pos_;
	XMFLOAT3		normal_;
	XMFLOAT2		uv_;
	XMFLOAT3		tangent_;
	XMFLOAT3		binormal_;
	VertexXYZNUVTB()
	{
	}
};

struct VertexXYZNUV
{
	XMFLOAT3		pos_;
	XMFLOAT3		normal_;
	XMFLOAT2		uv_;

	VertexXYZNUV(){}
	VertexXYZNUV(float x, float y, float z,
		float nx, float ny, float nz, float u, float v)
	{
		pos_.x = x;
		pos_.y = y;
		pos_.z = z;
		normal_.x = nx;
		normal_.y = ny;
		normal_.z = nz;
		uv_.x = u;
		uv_.y = v;
	}
	const static int FVF;
};

// Position //Normal
struct VertexXYZN
{
	XMFLOAT3		pos_;
	XMFLOAT3		normal_;
	VertexXYZN()
	{
	}
};


// Position
struct VertexXYZ
{
	XMFLOAT3		pos_;
	VertexXYZ()
	{
	}
};

//Normal
struct VertexN
{
	XMFLOAT3		normal_;

	VertexN()
	{
	}
};

//  UV
struct VertexUV
{
	XMFLOAT3		uv_;
	VertexUV()
	{
	}
};
// P Tangent, Binormal
struct VertexTB
{
	XMFLOAT3		tangent_;
	XMFLOAT3		binormal_;
	VertexTB()
	{
	}
};



#define VERTEX_FORMAT_XYZNUVTB		    "xyznuvtb"
#define VERTEX_FORMAT_XYZNUV		    "xyznuv"
#define VERTEX_FORMAT_XYZNUV2TB		    "xyznuv2tb"

#define VERTEX_FORMAT_XYZNUVIIIWWTB		"xyznuviiiwwtb"
#define VERTEX_FORMAT_XYZNUVIIIIWWWTB	"xyznuviiiiwwwtb"
#define VERTEX_FORMAT_XYZNUVP2          "xyznuvp2"
#define VERTEX_FORMAT_XYZNUVTBP2        "xyznuvtbp2"


// class VertexDeclaration
// {
// public:
// 	static D3DVERTEXELEMENT9 decl_xyznuvtb[6];
// 	static D3DVERTEXELEMENT9 decl_xyznuviiiwwtb[8];
// 	static D3DVERTEXELEMENT9 decl_xyznuviiiiwwwtb[8];
// 
// 	static D3DVERTEXELEMENT9 decl_xyz[2];
// 	static D3DVERTEXELEMENT9 decl_n[2];
// 	static D3DVERTEXELEMENT9 decl_uv[2];
// 	static D3DVERTEXELEMENT9 decl_tb[3];
// };
// 
// class MultiSteamVertexDeclaration
// {
// public:
// 	static D3DVERTEXELEMENT9 decl_xyznuvtb[6];
// 	static D3DVERTEXELEMENT9 decl_xyznuv[4];
// 	static D3DVERTEXELEMENT9 decl_xyzuv[3];
// 	static D3DVERTEXELEMENT9 decl_xyzn[3];
// 	//static D3DVERTEXELEMENT9 decl_xyznuviiiwwtb[8];
// 	//static D3DVERTEXELEMENT9 decl_xyznuviiiiwwwtb[8];
// };


void get_res_dir(std::string& str);

void get_program_dir(std::string& str);

#endif //BASE_DEFINE_H