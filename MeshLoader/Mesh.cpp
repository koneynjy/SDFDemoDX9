#include "Mesh.h"
#include "baseDefine.h"
#include <algorithm>

CMesh::CMesh()
{
}

CMesh::~CMesh()
{

}

//"D:/res/scene/common/wj/wjmy/slj_wjmy0390_wb.model"
//"D:/res/scene/common/wj/wjmy/slj_wjmy0390_wb.model"
bool CMesh::Init(std::string file_name, std::string dir, XMFLOAT3& pos)
{
	if (file_name.length() >= 1 &&
		file_name[file_name.length() - 1] == '/')
	{
		OBJLoader objloader;
		if (objloader.LoadDir(file_name, m_mesh_vertex, m_index_list))
		{
			submesh sub(this);
			sub.m_group_info.startIndex = 0;
			sub.m_group_info.primitives = m_index_list.size() / 3;
			sub.m_group_info.startVertex = 0;
			sub.m_group_info.vertices = m_mesh_vertex.size();
			m_submesh_list.push_back(sub);
			return true;
		}
		return false;
	}
	if (file_name.length() >= 4 &&
		file_name.substr(file_name.length() - 4, 4) == ".obj")
	{
		OBJLoader objloader;
		if (objloader.Load(file_name, m_mesh_vertex, m_index_list))
		{
			submesh sub(this);
			sub.m_group_info.startIndex = 0;
			sub.m_group_info.primitives = m_index_list.size() / 3;
			sub.m_group_info.startVertex = 0;
			sub.m_group_info.vertices = m_mesh_vertex.size();
			m_submesh_list.push_back(sub);
			XMStoreFloat4x4(&m_world_transform, XMMatrixTranslation(pos.x, pos.y, pos.z));
			return true;
		}
		return false;
	}
	if (file_name.length() >= 6 &&
		file_name.substr(file_name.length() - 6, 6) == ".model")
	{
		TiXmlDocument doc(file_name.c_str());
		if (!doc.LoadFile())
		{
			assert(0);
			return false;
		}
		TiXmlElement *vis = doc.RootElement()->FirstChildElement("nodelessVisual");
		if (vis == NULL)
		{
			vis = doc.RootElement()->FirstChildElement("nodefullVisual");
			if (vis == NULL)
			{
				assert(0);
			}
		}
		file_name = vis->FirstChild()->Value();
		this->dir = dir;
		if (!read_primitive(dir + file_name))
		{
			clear_cpu_data();
			return false;
		}
		read_visual_material(file_name);
		//create_gpu_data();

		//create_mesh();
		XMStoreFloat4x4(&m_world_transform, XMMatrixTranslation(pos.x, pos.y, pos.z));
		return true;
	}

	return false;
	
}

void CMesh::clear_cpu_data()
{
	VERTEX_LIST temp_vertex_list;
	INDEX_LIST  temp_index_list;
	m_vertex_list.swap(temp_vertex_list);
	m_index_list.swap(temp_index_list);
	m_vertex_list.clear();
	m_index_list.clear();

}