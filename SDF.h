#ifndef _SDF_H
#define _SDF_H
//#
#include "SDF/Config.h"
#include "Vertex.h"
#include "MeshLoader/Mesh.h"
struct SDFModel
{
public:
	MeshData *meshData;
	FDistanceFieldVolumeData *sdfData;
	FBoxSphereBounds *boxSphereBounds;
	SDFModel():sdfData(NULL){};
	SDFModel(CMesh& cmesh);
	SDFModel(std::vector<VertexPNT>& vert, std::vector<UINT> &ind);
	void GenerateSDF(
		float DistanceFieldResolutionScale, 
		bool bGenerateAsIfTwoSided
		);

	void GetSDFData(SDFFloat*& data, uint32&w, uint32&h, uint32&d);
	XMFLOAT3 GetOrigin();
	XMFLOAT3 GetBounds();
	XMFLOAT3 GetExtend();
	XMFLOAT3 GetModelExtend();
	float GetRes();
	static SDFModel Merge(SDFModel& m0, FVector& Pos0, SDFModel& m1, FVector& Pos1);
	~SDFModel();
};


#endif // !_SDF_H
