#include "SAO.h"
#include "Vertex.h"
SAO::SAO(UINT width, UINT height):
	mHeight(height), mWidth(width), 
	mRadius(0.5f), mBias(0.01f), mIntensity(1.0f), mProjScale(),
	mSAOMapBlurred1(0), mSAOMapBlurred0(0), mSAOMapSampled(0),
	SAOSampleFX(0), SAOBlurFX(0)
{
	SetRadius(2.0f);
	D3DVIEWPORT9 vp = {0, 0, mWidth, mHeight, 0.0f, 1.0f};
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFileEx(gd3dDevice, "FX/SAOSample.fx", 0, 0, 0, 
		0, 0, &SAOSampleFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	     
	SAOTech				= SAOSampleFX->GetTechniqueByName("SAOTech");
	CSZBuffer			= SAOSampleFX->GetParameterByName(0, "gCSZBuffer");
	GBuffer0			= SAOSampleFX->GetParameterByName(0, "gGBuffer0");
	View				= SAOSampleFX->GetParameterByName(0, "gView");
	Radius				= SAOSampleFX->GetParameterByName(0, "gRadius");
	Radius2				= SAOSampleFX->GetParameterByName(0, "gRadius2");
	FarPlane			= SAOSampleFX->GetParameterByName(0, "gFarPlane");
	Bias				= SAOSampleFX->GetParameterByName(0, "gBias");
	Intensity			= SAOSampleFX->GetParameterByName(0, "gIntensity");
	IntensityDivR6		= SAOSampleFX->GetParameterByName(0, "gIntensityDivR6");
	ProjScale			= SAOSampleFX->GetParameterByName(0, "gProjScale");
	Res					= SAOSampleFX->GetParameterByName(0, "gRes");

	errors = 0;
	HR(D3DXCreateEffectFromFileEx(gd3dDevice, "FX/SAOBlur.fx", 0, 0, 0, 
		0, 0, &SAOBlurFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	SAOBlurTech			= SAOBlurFX->GetTechniqueByName("SAOBlurTech");
	Source				= SAOBlurFX->GetParameterByName(0, "gSource");
	Axis				= SAOBlurFX->GetParameterByName(0, "gAxis");
	gRes				= SAOBlurFX->GetParameterByName(0, "gRes");
	
	mSAOMapBlurred0 = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_A8R8G8B8, false, D3DFMT_D24S8, vp, false);
	mSAOMapBlurred1 = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_A8R8G8B8, false, D3DFMT_D24S8, vp, false);
	mSAOMapSampled = new DrawableTex2D(mWidth, mHeight, 1, D3DFMT_A8R8G8B8, false, D3DFMT_D24S8, vp, false);

}

void SAO::SetRadius(float radius)
{
	mRadius = radius;
	mRadius2 = radius * radius;
	mRadius6 = mRadius2 * mRadius2 * mRadius2;
}

void SAO::OnLostDevice()
{
	mSAOMapSampled->onLostDevice();
	mSAOMapBlurred0->onLostDevice();
	mSAOMapBlurred1->onLostDevice();
	SAOSampleFX->OnLostDevice();
	SAOBlurFX->OnLostDevice();
}


void SAO::OnResetDevice()
{
	mSAOMapSampled->onResetDevice();
	mSAOMapBlurred0->onResetDevice();
	mSAOMapBlurred1->onResetDevice();
	SAOSampleFX->OnResetDevice();
	SAOBlurFX->OnResetDevice();
}

void SAO::SAOSample(IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib
					, Camera &camera, IDirect3DTexture9* zbuffer,IDirect3DTexture9* gbuffer0)
{
	gd3dDevice->BeginScene();
	gd3dDevice->SetRenderTarget(0, mSAOMapSampled->d3dSurface());
	HR(gd3dDevice->SetVertexDeclaration(VertexPT::Decl));
	HR(gd3dDevice->SetStreamSource(0, vb, 0, sizeof(VertexPT)));
	HR(gd3dDevice->SetIndices(ib));
	HR(SAOSampleFX->SetTechnique(SAOTech));
	HR(SAOSampleFX->SetTexture(CSZBuffer, zbuffer));
	HR(SAOSampleFX->SetTexture(GBuffer0, gbuffer0));
	HR(SAOSampleFX->SetMatrix(View, &camera.view()));
	HR(SAOSampleFX->SetFloat(Radius, mRadius));
	HR(SAOSampleFX->SetFloat(Radius2, mRadius2));
	HR(SAOSampleFX->SetFloat(Bias, mBias));
	HR(SAOSampleFX->SetFloat(Intensity, mIntensity));
	HR(SAOSampleFX->SetFloat(IntensityDivR6, mIntensity / mRadius6));
	//HR(SAOSampleFX->SetFloat(ProjScale, 500));
	HR(SAOSampleFX->SetFloat(ProjScale, 0.5f * mHeight / tan(0.5 * camera.GetFovY())));
	HR(SAOSampleFX->SetValue(Res, &D3DXVECTOR2(mWidth, mHeight), sizeof(D3DXVECTOR2)));
	HR(SAOSampleFX->SetValue(FarPlane, &D3DXVECTOR3(
		camera.GetFarWindowWidth() / camera.GetFarZ(),
		-camera.GetFarWindowHeight() / camera.GetFarZ(),
		camera.GetFarZ()), sizeof(D3DXVECTOR3)));
	UINT numPasses = 0;
	HR(SAOSampleFX->Begin(&numPasses, 0));
	HR(SAOSampleFX->BeginPass(0));
	HR(SAOSampleFX->CommitChanges());

	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,3,
		0,1));

	HR(SAOSampleFX->EndPass());
	HR(SAOSampleFX->End());
	gd3dDevice->EndScene();

}

void SAO::SAOBlur(IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	gd3dDevice->BeginScene();
	HR(gd3dDevice->SetVertexDeclaration(VertexPT::Decl));
	HR(gd3dDevice->SetStreamSource(0, vb, 0, sizeof(VertexPT)));
	HR(gd3dDevice->SetIndices(ib));
	HR(SAOBlurFX->SetTechnique(SAOBlurTech));
	HR(SAOBlurFX->SetValue(gRes, &D3DXVECTOR2(mWidth, mHeight), sizeof(D3DXVECTOR2)));
	//horizon
	gd3dDevice->SetRenderTarget(0, mSAOMapBlurred0->d3dSurface());
	HR(SAOBlurFX->SetValue(Axis, &D3DXVECTOR2(0, 1), sizeof(D3DXVECTOR2)));
	HR(SAOBlurFX->SetTexture(Source, mSAOMapSampled->d3dTex()));
	UINT numPasses = 0;
	HR(SAOBlurFX->Begin(&numPasses, 0));
	HR(SAOBlurFX->BeginPass(0));
	HR(SAOBlurFX->CommitChanges());

	
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,3,
		0,1));

	HR(SAOBlurFX->EndPass());
	HR(SAOBlurFX->End());

	//vertical
	gd3dDevice->SetRenderTarget(0, mSAOMapBlurred1->d3dSurface());
	HR(SAOBlurFX->SetValue(Axis, &D3DXVECTOR2(1, 0), sizeof(D3DXVECTOR2)));
	HR(SAOBlurFX->SetTexture(Source, mSAOMapBlurred0->d3dTex()));
	numPasses = 0;
	HR(SAOBlurFX->Begin(&numPasses, 0));
	HR(SAOBlurFX->BeginPass(0));
	HR(SAOBlurFX->CommitChanges());
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,4,
		0,2));

	HR(SAOBlurFX->EndPass());
	HR(SAOBlurFX->End());


	gd3dDevice->EndScene();

}


