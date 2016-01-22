#include "SDFShadow.h"
#include "GeometryGenerator.h"
#include "Vertex.h"

SDFShadow::SDFShadow(UINT width, UINT height)
:mSDFShadowMap(0), SDFShadowFX(0)
,mCullingVolumeVB(0),mCullingVolumeIB(0)
{
	mWidth = width;
	mHeight = height;

	D3DVIEWPORT9 vp = {0, 0, mWidth, mHeight, 0.0f, 1.0f};
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFileEx(gd3dDevice, "FX/SDFShadow.fx", 0, 0, 0, 
		0, 0, &SDFShadowFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	SDFShadowTech		= SDFShadowFX->GetTechniqueByName("SDFShadowTech");
	ViewProj			= SDFShadowFX->GetParameterByName(0, "gViewProj");
	LightDir			= SDFShadowFX->GetParameterByName(0, "gLightDir");
	EyePosW				= SDFShadowFX->GetParameterByName(0, "gEyePosW");
	FarClipDist			= SDFShadowFX->GetParameterByName(0, "gFarClipDist");
	SDFToWordInv0		= SDFShadowFX->GetParameterByName(0, "gSDFToWordInv0");
	SDFBounds0			= SDFShadowFX->GetParameterByName(0, "gSDFBounds0");
	SDFRes0				= SDFShadowFX->GetParameterByName(0, "gSDFRes0");
	SDF0				= SDFShadowFX->GetParameterByName(0, "gSDF0");
	DepthMap			= SDFShadowFX->GetParameterByName(0, "gDepthMap");
	GBuffer0			= SDFShadowFX->GetParameterByName(0, "gGBuffer0");

	mSDFShadowMap = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_R32F, false, D3DFMT_D24S8, vp, false);
}

void SDFShadow::GetCullingVolume(D3DXVECTOR3&center, D3DXVECTOR3& extend, 
					  D3DXVECTOR3& lightDir,D3DXMATRIX& worldMat)
{
	if(mCullingVolumeIB) mCullingVolumeIB->Release();
	if(mCullingVolumeVB) mCullingVolumeVB->Release();
	static D3DXVECTOR3 vt[8] =
	{
		D3DXVECTOR3(1, -1, -1),
		D3DXVECTOR3(1, 1, -1),
		D3DXVECTOR3(-1, 1, -1),
		D3DXVECTOR3(-1, -1, -1),
		D3DXVECTOR3(1, -1, 1),
		D3DXVECTOR3(1, 1, 1),
		D3DXVECTOR3(-1, 1, 1),
		D3DXVECTOR3(-1, -1, 1),
	};

	GeometryGenerator geoGen;

	D3DXVECTOR3 vert[8];
	for (int i = 0; i < 8; i++){
		vert[i] = center;
		for(int j = 0; j < 3; j++)
			vert[i][j] += vt[i][j] * extend[j];
		D3DXVECTOR4 tmpout;
		D3DXVec3Transform(&tmpout, &vert[i], &worldMat);
		for(int j = 0; j < 3; j ++) vert[i][j] = tmpout[j];
	}

	std::vector<D3DXVECTOR3> vertices;
	std::vector<USHORT> indices16;
	geoGen.CreateLightVolume(vertices, indices16, vert, lightDir);

	void *tvb, *tib;
	mVertexCount = vertices.size();
	HR(gd3dDevice->CreateVertexBuffer(
		mVertexCount * sizeof(VertexPos)
		,0
		,D3DFVF_XYZ
		,D3DPOOL_MANAGED
		,&mCullingVolumeVB
		,NULL));
	
	HR(mCullingVolumeVB->Lock(0, sizeof(VertexPos) * mVertexCount, & tvb, 0));
	memcpy(tvb, &vertices[0], sizeof(VertexPos) * mVertexCount);
	HR(mCullingVolumeVB->Unlock());

	mIndexCount = indices16.size();
	HR(gd3dDevice->CreateIndexBuffer(
		mIndexCount * sizeof(USHORT)
		,0
		,D3DFMT_INDEX16
		,D3DPOOL_MANAGED
		,&mCullingVolumeIB
		,NULL
		));
	HR(mCullingVolumeIB->Lock(0, sizeof(USHORT) * mIndexCount, & tib, 0));
	memcpy(tib, &indices16[0], sizeof(USHORT) * mIndexCount);
	HR(mCullingVolumeIB->Unlock());

}

void SDFShadow::OnLostDevice()
{
	mSDFShadowMap->onLostDevice();
	HR(SDFShadowFX->OnLostDevice());

}

void SDFShadow::OnResetDevice()
{
	mSDFShadowMap->onResetDevice();
	HR(SDFShadowFX->OnResetDevice());

}

void SDFShadow::SDFShadowPassBegin()
{
	HR(gd3dDevice->SetRenderTarget(0, mSDFShadowMap->d3dSurface()));
	HR(gd3dDevice->SetRenderTarget(1, NULL));
	HR(gd3dDevice->SetRenderTarget(2, NULL));
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0));
	HR(SDFShadowFX->SetTechnique(SDFShadowTech));
	HR(SDFShadowFX->SetMatrix(ViewProj, &gCamera->viewProj()));
	HR(SDFShadowFX->SetFloat(FarClipDist, gCamera->GetFarZ()));
	HR(SDFShadowFX->SetValue(EyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3)));
	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
	HR(gd3dDevice->SetStreamSource(0, mCullingVolumeVB, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetIndices(mCullingVolumeIB));
}

void SDFShadow::SDFShadowPassDraw()
{
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,mVertexCount,
		0,mIndexCount / 3));
}