#include "DeferredShading.h"
#include "GeometryGenerator.h"
#include "Vertex.h"

DeferredShading::DeferredShading(UINT width, UINT height)
{
	mWidth = width;
	mHeight = height;
	D3DVIEWPORT9 vp = {0, 0, mWidth, mHeight, 0.0f, 1.0f};
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFileEx(gd3dDevice, "FX/GBuffer.fx", 0, 0, 0, 
		0, 0, &GBufferFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	GBufferTech			= GBufferFX->GetTechniqueByName("GBufferTech");
	WorldInvTranspose   = GBufferFX->GetParameterByName(0, "gWorldInvTranspose");
	WorldViewProj		= GBufferFX->GetParameterByName(0, "gWorldViewProj");
	FarClipDist			= GBufferFX->GetParameterByName(0, "gFarClipDist");
	mtrl				= GBufferFX->GetParameterByName(0, "gMtrl");

	errors = 0;
	HR(D3DXCreateEffectFromFileEx(gd3dDevice, "FX/DeferredShading.fx", 0, 0, 0, 
		0, 0, &DeferredShadingFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	DeferredShadingTech	= DeferredShadingFX->GetTechniqueByName("DeferredShadingTech");
	DirLights			= DeferredShadingFX->GetParameterByName(0, "gDirLights");
	ViewInv				= DeferredShadingFX->GetParameterByName(0, "gViewInv");
	EyePosW				= DeferredShadingFX->GetParameterByName(0, "gEyePosW");
	amb					= DeferredShadingFX->GetParameterByName(0, "amb");
	gDepthMap			= DeferredShadingFX->GetParameterByName(0, "gDepthMap");
	gGBuffer0			= DeferredShadingFX->GetParameterByName(0, "gGBuffer0");
	gGBuffer1			= DeferredShadingFX->GetParameterByName(0, "gGBuffer1");
	gSDFShadow			= DeferredShadingFX->GetParameterByName(0, "gSDFShadow");


	mDepthMap = new DrawableTex2D(mWidth, mHeight, 5, D3DFMT_R32F, false, D3DFMT_D24S8, vp, false);
	mGBuffer0 = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_A8R8G8B8, false, D3DFMT_D24S8, vp, false);
	mGBuffer1 = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_A8R8G8B8, false, D3DFMT_D24S8, vp, false);
	
}

DeferredShading::~DeferredShading()
{
	ReleaseCOM(GBufferFX);
	ReleaseCOM(DeferredShadingFX);
	ReleaseCOM(mQuadVB);
	ReleaseCOM(mQuadIB);
	delete mDepthMap;
	delete mGBuffer0;
	delete mGBuffer1;
}

void DeferredShading::InitQuad()
{
	GeometryGenerator::MeshData quad;
	GeometryGenerator geoGen;
	geoGen.CreateFarPlaneQuad(quad, *gCamera);

	std::vector<D3DXVECTOR3> vertices(quad.Vertices.size());

	for (size_t i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i] = quad.Vertices[i].Position;
	}

	void *tvb, *tib;

	HR(gd3dDevice->CreateVertexBuffer(
		vertices.size() * sizeof(VertexPos)
		,0
		,D3DFVF_XYZ
		,D3DPOOL_MANAGED
		,&mQuadVB
		,NULL));
	vcount = vertices.size();
	HR(mQuadVB->Lock(0, sizeof(VertexPos) * vcount, & tvb, 0));
	memcpy(tvb, &vertices[0], sizeof(VertexPos) * vcount);
	HR(mQuadVB->Unlock());

	icount = quad.Indices.size();
	HR(gd3dDevice->CreateIndexBuffer(
		icount * sizeof(UINT)
		,0
		,D3DFMT_INDEX32
		,D3DPOOL_MANAGED
		,&mQuadIB
		,NULL
		));
	HR(mQuadIB->Lock(0, sizeof(UINT) * icount, & tib, 0));
	memcpy(tib, &quad.Indices[0], sizeof(UINT) * icount);
	HR(mQuadIB->Unlock());
}

void DeferredShading::GBufferBegin()
{
	HR(gd3dDevice->SetRenderTarget(0, mGBuffer0->d3dSurface()));
	HR(gd3dDevice->SetRenderTarget(1, mGBuffer1->d3dSurface()));
	HR(gd3dDevice->SetRenderTarget(2, mDepthMap->d3dSurface()));
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0));
	HR(gd3dDevice->BeginScene());
	HR(GBufferFX->SetTechnique(GBufferTech));
	HR(GBufferFX->SetFloat(FarClipDist, gCamera->GetFarZ()));
	UINT numPasses = 0;
	HR(GBufferFX->Begin(&numPasses, 0));
	HR(GBufferFX->BeginPass(0));
}

void DeferredShading::GBufferEnd()
{
	HR(GBufferFX->EndPass());
	HR(GBufferFX->End());
	HR(gd3dDevice->SetRenderTarget(1, NULL));
	HR(gd3dDevice->SetRenderTarget(2, NULL));
	HR(gd3dDevice->EndScene());

}

void DeferredShading::ShadingBegin()
{
	D3DXMATRIX viewInv;
	///////////////////////////g pass////////////////////////////////
	D3DXVECTOR3 right = gCamera->right();
	D3DXVECTOR3 up = gCamera->up();
	D3DXVECTOR3 look = gCamera->look();
	viewInv(0,0) = right.x;viewInv(1,0) = up.x;viewInv(2,0) = look.x;
	viewInv(0,1) = right.y;viewInv(1,1) = up.y;viewInv(2,1) = look.y;
	viewInv(0,2) = right.z;viewInv(1,2) = up.z;viewInv(2,2) = look.z;
	HR(gd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
	HR(gd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false));
	HR(DeferredShadingFX->SetTechnique(DeferredShadingTech));
	HR(DeferredShadingFX->SetMatrix(ViewInv, &viewInv));
	HR(DeferredShadingFX->SetValue(EyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3)));
	HR(DeferredShadingFX->SetVector(amb, &D3DXVECTOR4(0.2f, 0.2f, 0.2f, 0.0f)));
	HR(DeferredShadingFX->SetTexture(gDepthMap, mDepthMap->d3dTex()));
	HR(DeferredShadingFX->SetTexture(gGBuffer0, mGBuffer0->d3dTex()));
	HR(DeferredShadingFX->SetTexture(gGBuffer1, mGBuffer1->d3dTex()));

	UINT numPasses = 0;
	HR(DeferredShadingFX->Begin(&numPasses, 0));
	HR(DeferredShadingFX->BeginPass(0));
}

void DeferredShading::ShadingEnd()
{
	HR(DeferredShadingFX->CommitChanges());
	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
	HR(gd3dDevice->SetStreamSource(0, mQuadVB, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetIndices(mQuadIB));
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,4,
		0,2));
	HR(DeferredShadingFX->EndPass());
	HR(DeferredShadingFX->End());
	
	HR(gd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
	HR(gd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true));
}

void DeferredShading::onLostDeivce()
{
	mDepthMap->onLostDevice();
	mGBuffer0->onLostDevice();
	mGBuffer1->onLostDevice();

	HR(GBufferFX->OnLostDevice());
	HR(DeferredShadingFX->OnLostDevice());

}

void DeferredShading::onResetDevice()
{
	mDepthMap->onResetDevice();
	mGBuffer0->onResetDevice();
	mGBuffer1->onResetDevice();

	HR(GBufferFX->OnResetDevice());
	HR(DeferredShadingFX->OnResetDevice());
}

