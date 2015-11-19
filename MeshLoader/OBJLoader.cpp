#include "LoadOBJ.h"
#include <map>
#include "io.h"
using std::map;
using std::vector;
OBJLoader::OBJLoader(){}
OBJLoader::~OBJLoader(){}
typedef unsigned long long ull;

struct ID{
	UINT vid, nid, tid;
	inline bool operator<(const ID& r) const
	{
		if (vid < r.vid) return true;
		if (vid == r.vid && nid < r.nid) return true;
		if (vid == r.vid && nid == r.nid && tid < r.tid) return true;
		return false;
	}
};

bool OBJLoader::LoadDir(string file_name,
	std::vector<VertexXYZNUV>& vertices,
	std::vector<UINT>& indices)
{
	string path = file_name + "*.*";
	_finddata_t file;
	long lf;
	if ((lf = _findfirst(path.c_str(), &file)) == -1l)//_findfirst返回的是long型; long __cdecl _findfirst(const char *, struct _finddata_t *)
		return false;
	else
	{
		while (_findnext(lf, &file) == 0)//int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1
		{
			if (file.attrib == _A_SUBDIR)
			{
				string name(file.name);
				if (name != ".." && name != ".")
					Load(file_name + name + "/" + name + ".obj",vertices, indices);
			}
		}
	}
	_findclose(lf);
	return true;
}

bool OBJLoader::Load(string file_name,
	vector<VertexXYZNUV>& vertices,
	vector<UINT>& indices)
{
	vector<XMFLOAT3> vb;
	vector<XMFLOAT3> nb;
	vector<XMFLOAT2> tb;
	map<ID, USHORT> ibmap;
	vector<ID> ib;
	ifstream fin(file_name);
	//vector<XMFLOAT3> testvb0;
	//vector<XMFLOAT3> testvb1;
	char str[256];
	fin.getline(str, 256);
	while (!fin.fail())
	{
		string s(str);
		if (s.length() >=2 && s[1] == ' ')
		{
			string sub = s.substr(2, s.length() - 2);
			if (s[0] == 'v')
			{
				XMFLOAT3 p;
				sscanf(sub.c_str(), "%f%f%f", &p.x, &p.y, &p.z);
				vb.push_back(p);
			}
			else if (s[0] == 'f')
			{
				UINT i[9];
				sscanf(sub.c_str(), "%u/%u/%u %u/%u/%u %u/%u/%u", 
					i, i + 1, i + 2, i + 3, i + 4, i + 5, i + 6, i + 7, i + 8);
				for (int ii = 0; ii < 9; ii+=3)
				{
					//testvb0.push_back(vb[i[ii] - 1]);
					ID u;
					u.vid = i[ii];
					u.tid = i[ii + 1];
					u.nid = i[ii + 2];
					ib.push_back(u);
					ibmap[u] = 0;
				}
			}
		}

		else if (s.length() >= 3 && s[2] == ' ' && s[0] == 'v')
		{
			string sub = s.substr(3, s.length() - 3);
			if (s[1] == 'n')
			{
				XMFLOAT3 p;
				sscanf(sub.c_str(), "%f%f%f", &p.x, &p.y, &p.z);
				nb.push_back(p);
			}
			else if (s[1] == 't')
			{
				XMFLOAT2 p;
				sscanf(sub.c_str(), "%f%f", &p.x, &p.y);
				tb.push_back(p);
			}
		}
		fin.clear(ios::goodbit);
		fin.getline(str, 256);
	}

	USHORT id = vertices.size();
	for (auto i = ibmap.begin(); i != ibmap.end(); i++)
	{
		ID u = i->first;
		i->second = id++;
		VertexXYZNUV vert;
		vert.pos_ =		vb[u.vid - 1];
		vert.uv_ =		tb[u.tid - 1];
		vert.normal_ =	nb[u.nid - 1];
		vertices.push_back(vert);
	}

	for (auto u : ib)
	{
		indices.push_back(ibmap[u]);
		//testvb1.push_back(vertices[indices.back()].pos_);
	}

	return true;
}