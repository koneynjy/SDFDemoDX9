#include "ModelFileParse.h"

#include <assert.h>
#include <memory.h>
#include <direct.h>
#include "Mesh.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tinyxml.h"
//#pragma optimize("", off)
extern bool  bLoadTexture;

int CMesh::pad(unsigned long size, int padding)
{
	return ((padding - size) % padding);
}

//file_name = "D:/res/scene/common/zw/zwshu/ycj_zwshu1020_2366"
//"D:/res/scene/common/zw/zwshu/xin_zwshu_0010_2924"
//file_name = "D:/res/scene/common/db/dbsk/xsc_dbsk0010_wb_large"
bool CMesh::read_primitive(const std::string& file_name, const std::string& sub_dir)
{
	FILE *fp;
	std::string full_path;
	get_res_dir(full_path);
	if (!sub_dir.empty())
		full_path += sub_dir;
	full_path += file_name;
	full_path += ".primitives"; //primitive file

	fp = fopen(full_path.c_str(), "rb");
	if (!fp)
	{
		assert(0);
		//Log::get_instance()->LogMessage("open failed in read_primitive");
		return false;
	}

	int MAGIC_NUMBER = 0x42a14e65;
	unsigned long magic_number = 0;

	fread(&magic_number, 4, 1, fp);

	if (magic_number != MAGIC_NUMBER)
	{
		assert(0);
		//Log::get_instance()->LogMessage("magic number error read_primitive");
		fclose(fp);
		return false;
	}

	unsigned long index_size = 0;
	fseek(fp, -4, SEEK_END);
	fread(&index_size, 4, 1, fp);
	assert((index_size % 4) == 0);


	fseek(fp, -4 - index_size, SEEK_END);
	unsigned long readsize = 0;
	unsigned long calc_size = 0;
	calc_size += sizeof(magic_number);

	sec_info_vector sec_list;
	unsigned long index = 0;

	char sec_file_name[MAX_PATH];//一般不会超过256;
	while (readsize < index_size)
	{
		unsigned long nums[6];
		fread(nums, 4, 6, fp);
		readsize += 4 * 6;
		unsigned long sec_size = nums[0];
		unsigned long secname_size = nums[5];
		std::string scename;
		memset(sec_file_name, 0, MAX_PATH);
		fread(sec_file_name, 1, secname_size, fp);
		readsize += secname_size;

		int pad_size = pad(secname_size);
		if (pad_size)
		{
			int pad_size_content;
			fread(&pad_size_content, 1, pad_size, fp); // pad to 4bytes
			readsize += pad_size;
		}

		sec_info new_sec;
		new_sec.sec_file_name = sec_file_name;
		new_sec.sec_size = sec_size;
		new_sec.offset = calc_size;
		sec_list.push_back(new_sec);

		calc_size += sec_size;
		calc_size += pad(sec_size);

		index++;
	}


	calc_size += index_size;
	calc_size += 4;

	fseek(fp, 0, SEEK_END);
	unsigned long fsize = ftell(fp);
	assert(fsize == calc_size);

	for (size_t i = 0; i < sec_list.size(); i++)
	{
		if (sec_list[i].sec_file_name.find(VERTEX) != std::string::npos ||
			sec_list[i].sec_file_name == VERTEX)
		{
			if (!read_vertex(fp, sec_list[i]))
			{
				assert(0);
				fclose(fp);
				return false;
			}
		}
		else if (sec_list[i].sec_file_name.find(INDEX) != std::string::npos ||
			sec_list[i].sec_file_name == INDEX)
		{
			if (!read_indexs(fp, sec_list[i]))
			{
				//assert(0);
				fclose(fp);
				return false;
			}
		}
	}

	fclose(fp);
	return true;
}


void unpackNormal(uint32 packNromal, XMFLOAT3& ret)
{
	int z = int(packNromal) >> 22;
	int y = int(packNromal << 10) >> 21;
	int x = int(packNromal << 21) >> 21;

	ret.x = (float)(x) / 1023.0f;
	ret.y = (float)(y) / 1023.0f;
	ret.z = (float)(z) / 511.0f;
}

bool CMesh::read_vertex(FILE* fp, sec_info& sec)
{
	if (!fp)
	{
		assert(0);
		//Log::get_instance()->LogMessage("CMesh::read_vertex file is null");
		return false;
	}
	char vertexformat[128];
	int  vertex_number = 0;
	memset(vertexformat, 0, 128);
	fseek(fp, sec.offset, SEEK_SET);
	fread(&vertexformat, 1, 64, fp);
	fread(&vertex_number, sizeof(int), 1, fp);
	m_vertex_format = vertexformat;

	std::string vertexFormatClean = "";
	for (size_t i = 0; i<strlen(vertexformat); i++)
	{
		if ((vertexformat[i] >= 'a' && vertexformat[i] <= 'z') || (vertexformat[i] == '2'))
		{
			vertexFormatClean += vertexformat[i];
		}
		else
		{
			break;
		}
	}

	bool b_uv2 = false;
	if (vertexFormatClean.find("2uv") != std::string::npos || vertexFormatClean.find("uv2") != std::string::npos)
	{
		b_uv2 = true;
	}


	if (vertexFormatClean != VERTEX_FORMAT_XYZNUVTB &&
		vertexFormatClean != VERTEX_FORMAT_XYZNUV &&
		vertexFormatClean != VERTEX_FORMAT_XYZNUV2TB&&
		vertexFormatClean != VERTEX_FORMAT_XYZNUVIIIWWTB&&
		vertexFormatClean != VERTEX_FORMAT_XYZNUVIIIIWWWTB&&
		vertexFormatClean != VERTEX_FORMAT_XYZNUVP2&&
		vertexFormatClean != VERTEX_FORMAT_XYZNUVTBP2)
	{
		std::string log = "vertex format has problem: ";
		log += m_vertex_format;

		//assert(0);
		//Log::get_instance()->LogMessage(log);
		return false;
	}

	XMFLOAT3 pos;
	uint32 uint_normal;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT2 uv2;
	uint32 uint_tangent;
	XMFLOAT3 tangent;
	uint32 uint_bitangent;
	XMFLOAT3 bitangent;
	unsigned char iw[100];
	if (VERTEX_FORMAT_XYZNUVTB == m_vertex_format)
	{
		for (int i = 0; i < vertex_number; i++)
		{
			/*
			VertexXYZNUVTB new_vertex;
			fread(&new_vertex.pos_, sizeof(D3DXVECTOR3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(D3DXVECTOR2), 1, fp);
			fread(&uint_tangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_tangent, new_vertex.tangent_);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_bitangent, new_vertex.binormal_);
			m_vertex_list.push_back(new_vertex);
			*/
			//m_mesh_vertex
			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);
			fread(&uint_tangent, sizeof(uint32), 1, fp);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);
			m_mesh_vertex.push_back(new_vertex);
		}
	}
	else if (VERTEX_FORMAT_XYZNUV == m_vertex_format)
	{
		for (int i = 0; i < vertex_number; i++)
		{
			/*
			VertexXYZNUVTB new_vertex;
			fread(&new_vertex.pos_, sizeof(D3DXVECTOR3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(D3DXVECTOR2), 1, fp);
			//just make data
			new_vertex.tangent_ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			new_vertex.binormal_ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			// 		fread(&uint_tangent, sizeof(uint32), 1, fp);
			// 		unpackNormal(uint_tangent, new_vertex.tangent_);
			// 		fread(&uint_bitangent, sizeof(uint32), 1, fp);
			// 		unpackNormal(uint_bitangent, new_vertex.binormal_);
			m_vertex_list.push_back(new_vertex);
			*/

			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);
			m_mesh_vertex.push_back(new_vertex);
		}
	}

	else if (VERTEX_FORMAT_XYZNUV2TB == m_vertex_format)
	{
		for (int i = 0; i < vertex_number; i++)
		{
			/*
			VertexXYZNUVTB new_vertex;
			fread(&new_vertex.pos_, sizeof(D3DXVECTOR3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(D3DXVECTOR2), 1, fp);
			//just read it
			fread(uv2, sizeof(D3DXVECTOR2), 1, fp);

			fread(&uint_tangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_tangent, new_vertex.tangent_);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_bitangent, new_vertex.binormal_);
			m_vertex_list.push_back(new_vertex);
			*/

			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);

			fread(&uv2, sizeof(XMFLOAT2), 1, fp);
			fread(&uint_tangent, sizeof(uint32), 1, fp);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);

			m_mesh_vertex.push_back(new_vertex);
		}
	}
	else if (VERTEX_FORMAT_XYZNUVIIIWWTB == m_vertex_format)
	{
		for (int i = 0; i < vertex_number; i++)
		{
			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);
			fread(&uint_normal, sizeof(uint32), 1, fp);
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);

			fread(iw, sizeof(uchar), 5, fp);
			fread(&uint_tangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_tangent, normal);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);
			unpackNormal(uint_bitangent, normal);

			m_mesh_vertex.push_back(new_vertex);
		}
	}
	else if (VERTEX_FORMAT_XYZNUVIIIIWWWTB == m_vertex_format)
	{
		assert(0);
		return false;
	}
	else if (VERTEX_FORMAT_XYZNUVP2 == m_vertex_format)
	{
		//return false;
		for (int i = 0; i < vertex_number; i++)
		{
			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);  //12
			fread(&uint_normal, sizeof(uint32), 1, fp);           //4
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);   //8

			fread(iw, sizeof(uchar), 16, fp);    //p 8 bit
			m_mesh_vertex.push_back(new_vertex);
		}
	}
	else if (VERTEX_FORMAT_XYZNUVTBP2 == m_vertex_format)
	{
		for (int i = 0; i < vertex_number; i++)
		{
			VertexXYZNUV new_vertex;
			fread(&new_vertex.pos_, sizeof(XMFLOAT3), 1, fp);  //12
			fread(&uint_normal, sizeof(uint32), 1, fp);           //4
			unpackNormal(uint_normal, new_vertex.normal_);
			fread(&new_vertex.uv_, sizeof(XMFLOAT2), 1, fp);   //8

			//useless
			fread(&uint_tangent, sizeof(uint32), 1, fp);
			fread(&uint_bitangent, sizeof(uint32), 1, fp);
			fread(iw, sizeof(uchar), 16, fp);    //p 8 bit
			m_mesh_vertex.push_back(new_vertex);
		}

	}
	else
	{
		std::string log = "not process this format: ";
		log += m_vertex_format;
		//Log::get_instance()->LogMessage(log);
		return false;
	}

	if (m_mesh_vertex.size() <= 0)
		return false;

	return true;

	/*
	if (b_uv2) //not use this
	{
	fread(&pos, sizeof(D3DXVECTOR3), 1, fp);
	fread(&uint_normal, sizeof(uint32), 1, fp);
	unpackNormal(uint_normal, normal);
	fread(uv, sizeof(D3DXVECTOR2), 1, fp);
	fread(uv2, sizeof(D3DXVECTOR2), 1, fp);
	fread(&uint_tangent, sizeof(uint32), 1, fp);
	unpackNormal(uint_tangent, tangent);
	fread(&uint_bitangent, sizeof(uint32), 1, fp);
	unpackNormal(uint_bitangent, bitangent);
	}
	else
	{
	for (int i = 0; i < vertex_number; i++)
	{
	VertexXYZNUVTB new_vertex;
	fread(&new_vertex.pos_, sizeof(D3DXVECTOR3), 1, fp);
	fread(&uint_normal, sizeof(uint32), 1, fp);
	unpackNormal(uint_normal, new_vertex.normal_);
	fread(&new_vertex.uv_, sizeof(D3DXVECTOR2), 1, fp);
	fread(&uint_tangent, sizeof(uint32), 1, fp);
	unpackNormal(uint_tangent, new_vertex.tangent_);
	fread(&uint_bitangent, sizeof(uint32), 1, fp);
	unpackNormal(uint_bitangent, new_vertex.binormal_);
	m_vertex_list.push_back(new_vertex);
	}
	}
	*/
}


bool CMesh::read_indexs(FILE* fp, sec_info& sec)
{
	if (!fp)
	{
		//Log::get_instance()->LogMessage("CMesh::read_indexs file is null");
		return false;
	}
	char indexFormat[128];
	int  index_number = 0;
	int  group_number = 0;
	memset(indexFormat, 0, 128);
	fseek(fp, sec.offset, SEEK_SET);
	fread(&indexFormat, 1, 64, fp);
	fread(&index_number, sizeof(int), 1, fp);
	fread(&group_number, sizeof(int), 1, fp);

	unsigned short index;
	for (int i = 0; i < index_number; i++)
	{
		fread(&index, sizeof(unsigned short), 1, fp);
		m_index_list.push_back(index);
	}
	for (int i = 0; i < group_number; i++)
	{
		submesh new_submesh(this);

		fread(&new_submesh.m_group_info.startIndex, sizeof(int), 1, fp);
		fread(&new_submesh.m_group_info.primitives, sizeof(int), 1, fp);
		fread(&new_submesh.m_group_info.startVertex, sizeof(int), 1, fp);
		fread(&new_submesh.m_group_info.vertices, sizeof(int), 1, fp);

		m_submesh_list.push_back(new_submesh);
	}

	if (m_index_list.size() <= 0)
		return false;
	return true;
}

void CMesh::read_visual_material(const std::string& file_name)
{
	std::string full_path(this->dir);
	//get_res_dir(full_path);
	full_path += file_name;
	full_path += ".visual"; //visual file

	TiXmlDocument sceneDoc(full_path.c_str());
	if (sceneDoc.LoadFile())
	{
		TiXmlElement* rootElement = sceneDoc.RootElement();
		TiXmlElement* renderSetElement = rootElement->FirstChildElement();
		while (renderSetElement)
		{
			std::string value = renderSetElement->Value();
			if (RENDERSET_ELEMENT == value)
			{
				parse_renderset(renderSetElement);
				break;
			}
			renderSetElement = renderSetElement->NextSiblingElement();
		}
	}
}

void CMesh::parse_renderset(TiXmlElement* renderSetElement)
{
	if (!renderSetElement)
		return;

	TiXmlElement* geometryElement = renderSetElement->FirstChildElement();
	while (geometryElement)
	{
		std::string geometry_value = geometryElement->Value();
		if (GEOMETRY_ELEMENT == geometry_value)
		{
			TiXmlElement* primitiveGroup = geometryElement->FirstChildElement();
			int index = 0;
			while (primitiveGroup)
			{
				std::string primitiveGroup_value = primitiveGroup->Value();
				if (PRIMITIVEGROUP_ELEMENT == primitiveGroup_value)
				{
					TiXmlElement* material_element = primitiveGroup->FirstChildElement();

					while (material_element)
					{
						parse_material(material_element, index);
						material_element = material_element->NextSiblingElement();

					}
					index++;
				}
				primitiveGroup = primitiveGroup->NextSiblingElement();
			}
		}
		geometryElement = geometryElement->NextSiblingElement();
	}

}

void CMesh::parse_material(TiXmlElement* material_element, int index)
{
	if (!material_element)
		return;
	if (m_submesh_list.size() <= index)
	{
		assert(0);
		//Log::get_instance()->LogMessage("CMesh::parse_material error: index is larger than the submesh list size");
		return;
	}
	submesh& sub_mesh = m_submesh_list[index];

	TiXmlElement* mate_child_ele = material_element->FirstChildElement();
	while (mate_child_ele)
	{
		std::string text = mate_child_ele->GetText();
		if (DIFFUSETEXMAP == text)
		{
			TiXmlElement* Texture = mate_child_ele->FirstChildElement();
			if (Texture)
			{
				const char* text = Texture->GetText();
				if (text)
				{
					std::string texture_file_name = this->dir + text;
					sub_mesh.m_diffuse.LoadTGA(texture_file_name);
				}
			}
		}
		else if (NORMALTEXMAP == text)
		{
			TiXmlElement* Texture = mate_child_ele->FirstChildElement();
			if (Texture)
			{
				const char* text = Texture->GetText();
				if (text)
				{
					std::string texture_file_name = text;
				}
			}
		}
		else if (SPECULARTEXMAP == text)
		{
			TiXmlElement* Texture = mate_child_ele->FirstChildElement();
			if (Texture)
			{
				const char* text = Texture->GetText();
				if (text)
				{
					std::string texture_file_name = text;
				}
			}
		}
		else if (DIFFUSEPOWER == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string diffuse_power = float_value->GetText();
				sub_mesh.m_mat.m_diffuse_power = atof(diffuse_power.c_str());
			}
		}
		else if (SPECULARTEXS == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string diffuse_power = float_value->GetText();
				sub_mesh.m_mat.m_specular_tex_s = atof(diffuse_power.c_str());
			}
		}
		else if (SPECULARPOWER == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string diffuse_power = float_value->GetText();
				sub_mesh.m_mat.m_specular_power = atof(diffuse_power.c_str());
			}
		}
		else if (ALPHATEST == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string alphaTest = float_value->GetText();
				if (sub_mesh.m_diffuse.image != 0)
					sub_mesh.m_alphaTest = alphaTest == "true";
			}
		}
		else if (TWOSIDED == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string alphaTest = float_value->GetText();
				sub_mesh.m_twoSided =  atoi(alphaTest.c_str()) == 1;
			}
		}
		else if (ALPHAREF == text)
		{
			TiXmlElement* float_value = mate_child_ele->FirstChildElement();
			if (float_value)
			{
				std::string alphaTest = float_value->GetText();
				sub_mesh.m_alphaRef = atoi(alphaTest.c_str());
			}
		}

		mate_child_ele = mate_child_ele->NextSiblingElement();
	}
}