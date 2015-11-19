#ifndef MESH_H
#define MESH_H
#include "baseDefine.h"
#include "tinyxml.h"
#include "submesh.h"
#include <string>
#include <vector>	
#include "ModelFileParse.h"
#include "TGALoader.h"
#include "LoadOBJ.h"
using std::vector;
using std::string;
typedef vector<submesh> SUBMESH_LIST;
typedef vector<VertexXYZNUVTB> VERTEX_LIST;
typedef vector<VertexXYZNUV>   MESH_VERTEX;
typedef vector<UINT> INDEX_LIST;

class CMesh
{
public:
	CMesh();
	~CMesh();

	bool Init(string file_name, string dir, XMFLOAT3 & pos);
	int  getid(){ return m_id; }
	VERTEX_LIST& GetVertexList() { return m_vertex_list; }
	INDEX_LIST& GetIndexList(){ return m_index_list; }
	MESH_VERTEX& GetMeshVertex(){ return m_mesh_vertex; };
	SUBMESH_LIST& GetSubMeshList(){ return m_submesh_list; };
	XMFLOAT4X4& GetWorldTrans(){ return m_world_transform; };
protected:
	bool read_primitive(const string& file_name, const string& sub_dir = "");
	void read_visual_material(const string& file_name);
protected:
	bool create_mesh();
	void parse_renderset(TiXmlElement* renderSetElement);
	void parse_material(TiXmlElement* material_element, int index);
	int  pad(unsigned long  size, int padding = 4);
	bool read_vertex(FILE* fp, sec_info& sec);
	bool read_indexs(FILE* fp, sec_info& sec);
	void clear_cpu_data();
	
protected:
	//CUP DATA
	XMFLOAT4X4				m_world_transform;
	SUBMESH_LIST			m_submesh_list;
	VERTEX_LIST				m_vertex_list;
	INDEX_LIST				m_index_list;
	MESH_VERTEX             m_mesh_vertex;
	string					m_vertex_format;
	int                          m_id;
	string					dir;
};


#endif //MESH_H