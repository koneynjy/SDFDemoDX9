#include "submesh.h"
#include "Mesh.h"

material::material()
{
// 	m_diffuse = NULL;
// 	m_normal = NULL;
// 	m_specular = NULL;
	m_specular_power = 20.0f;
	m_diffuse_power = 1.0f;
	m_specular_tex_s = 1.0f;
}

material::~material()
{

}

submesh::submesh(CMesh* parent) :m_twoSided(false), m_alphaTest(false)
{
	m_parent = parent;
}

submesh::~submesh()
{
	m_parent = NULL;
}
