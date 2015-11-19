#ifndef _OBJ_H
#define _OBJ_H
#include <string>
#include <vector>	
#include <fstream>
#include "baseDefine.h"
using std::vector;
using std::string;
using std::fstream;
using std::ifstream;
using std::ios;

class OBJLoader
{
public:
	OBJLoader();
	~OBJLoader();

	bool Load(string file_name,
		std::vector<VertexXYZNUV>& vertices,
		std::vector<UINT>& indices);
	bool LoadDir(string file_name,
		std::vector<VertexXYZNUV>& vertices,
		std::vector<UINT>& indices);

};


#endif