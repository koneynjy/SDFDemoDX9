#include "SDF.h"
#include "SDF/MeshUtilities.h"
//#pragma optimize("", off)

SDFModel::SDFModel(CMesh& cmesh)
{
	meshData = new MeshData();
	MeshVerts &vertices = meshData->Vertices;
	MeshUVs &uvs = meshData->UVs;
	MeshTries &tris = meshData->Indices;
	MeshMats &mats = meshData->Mats;

	SUBMESH_LIST& submesh_list = cmesh.GetSubMeshList();
	MESH_VERTEX& vb = cmesh.GetMeshVertex();
	INDEX_LIST& ib = cmesh.GetIndexList();
 	vertices.resize(vb.size());
	uvs.resize(vb.size());
	tris.resize(ib.size() / 3);
	mats.resize(submesh_list.size());
	
	for (int k = 0; k < submesh_list.size(); k++)
	{
		for (uint32 i = submesh_list[k].getStartVertex(); i < submesh_list[k].getEndVertex(); i++)
		{
			vertices[i] = *(FVector*)&vb[i].pos_;
			if (submesh_list[k].m_alphaTest) //uv is not used when alpha test is not enabled
				uvs[i] = *(FVector2D*)&vb[i].uv_;
		}

		for (uint32 i = submesh_list[k].getStartIndex(); i < submesh_list[k].getEndIndex(); i ++)
		{
			tris[i / 3].indices[i % 3] = ib[i];
			tris[i / 3].material = k;//materail group index
		}
		mats[k].alphaTest = submesh_list[k].m_alphaTest;
		mats[k].twoSided = submesh_list[k].m_twoSided;
		if (submesh_list[k].m_alphaTest) {
			mats[k].alphaRef = submesh_list[k].m_alphaRef;
			mats[k].diffuse = *(FTexture*)&submesh_list[k].m_diffuse;
		}
	}

	boxSphereBounds = new FBoxSphereBounds();
	GenerateBoxSphereBounds(boxSphereBounds, meshData);
	sdfData = new FDistanceFieldVolumeData(boxSphereBounds->GetBox());
}

SDFModel::SDFModel(std::vector<VertexPNT>& vert, std::vector<UINT> &ind)
{
	meshData = new MeshData();
	MeshVerts &vertices = meshData->Vertices;
	MeshTries &tris = meshData->Indices;
	vertices.resize(vert.size());
	tris.resize(ind.size() / 3);

	for (uint32 i = 0; i < vertices.size(); i++)
	{
		vertices[i] = *(FVector*)&vert[i].pos;
	}

	for (uint32 i = 0; i < tris.size(); i++)
	{
		tris[i].indices[0] = ind[i * 3];
		tris[i].indices[1] = ind[i * 3 + 1];
		tris[i].indices[2] = ind[i * 3 + 2];
	}
	boxSphereBounds = new FBoxSphereBounds();
	GenerateBoxSphereBounds(boxSphereBounds, meshData);
	sdfData = new FDistanceFieldVolumeData(boxSphereBounds->GetBox());
}

SDFModel::~SDFModel()
{
	delete meshData;
	delete sdfData;
	delete boxSphereBounds;
}

void SDFModel::GenerateSDF(float DistanceFieldResolutionScale, bool bGenerateAsIfTwoSided)
{
	GenerateSignedDistanceFieldVolumeData(
		*meshData
		, *boxSphereBounds
		, DistanceFieldResolutionScale
		, bGenerateAsIfTwoSided
		, *sdfData);
}

void SDFModel::GetSDFData(SDFFloat*& data, uint32&w, uint32&h, uint32&d)
{
	sdfData->GetDistanceFieldVolumeData(data);
	w = sdfData->Size.X;
	h = sdfData->Size.Y;
	d = sdfData->Size.Z;
}

XMFLOAT3 SDFModel::GetOrigin()
{
	return *(XMFLOAT3*)&boxSphereBounds->Origin;
}

XMFLOAT3 SDFModel::GetBounds()
{
	FVector ret = sdfData->LocalBoundingBox.Max - sdfData->LocalBoundingBox.Min;
	return *(XMFLOAT3*)&ret;
}

XMFLOAT3 SDFModel::GetExtend()
{
	FVector ret = (sdfData->LocalBoundingBox.Max - sdfData->LocalBoundingBox.Min) / 2;
	return *(XMFLOAT3*)&ret;
}

XMFLOAT3 SDFModel::GetModelExtend()
{
	FVector ret = boxSphereBounds->BoxExtent;
	return *(XMFLOAT3*)&ret;
}

float SDFModel::GetRes()
{
	return 0.0f;
}

SDFModel SDFModel::Merge(SDFModel& m0, FVector& Pos0, SDFModel& m1, FVector& Pos1)
{
	return SDFModel();
}