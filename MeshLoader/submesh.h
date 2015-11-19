#ifndef SUBMESH_H
#define SUBMESH_H
#include "TGALoader.h"

class material
{
public:
	material();
	~material();
public:
// 	IDirect3DTexture9* m_diffuse;
// 	IDirect3DTexture9* m_normal;
// 	IDirect3DTexture9* m_specular;
	float			   m_specular_power;
	float			   m_diffuse_power;
	float			   m_specular_tex_s;
};

struct group_info
{
	int startIndex;
	int primitives;
	int startVertex;
	int vertices;
};

class CMesh;

class submesh
{
	friend class CMesh;
	friend class CMyMesh;
public:
	submesh(CMesh* parent);
	inline int getStartIndex(){ return m_group_info.startIndex; }
	inline int getEndIndex(){ return m_group_info.startIndex + m_group_info.primitives * 3; }
	inline int getStartVertex(){ return m_group_info.startVertex; }
	inline int getEndVertex(){ return m_group_info.startVertex+ m_group_info.vertices; }
	~submesh();
	bool		 m_twoSided;
	bool		 m_alphaTest;
	uchar		 m_alphaRef;
	CTGALoader	 m_diffuse;
	material	 m_mat;
	group_info   m_group_info;
	CMesh*		 m_parent;
};

#endif //SUBMESH_H