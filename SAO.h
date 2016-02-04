#ifndef _SAO
#define _SAO
#include "d3dUtil.h"
#include "Camera.h"
#include "DrawableTex2D.h"

class SAO
{
public:
	SAO(UINT width, UINT height);
	~SAO();

	void SetRadius(float radius);
	void OnLostDevice();
	void OnResetDevice();

	void SAOSample(IDirect3DVertexBuffer9* vb, 
		IDirect3DIndexBuffer9* ib, Camera &camera,
		IDirect3DTexture9* zbuffer, IDirect3DTexture9* gbuffer0);
	void SAOBlur(IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	DrawableTex2D* mSAOMapBlurred0;
	DrawableTex2D* mSAOMapBlurred1;
	DrawableTex2D* mSAOMapSampled;
	IDirect3DVertexBuffer9* mQuadVB;
	IDirect3DIndexBuffer9* mQuadIB;


	ID3DXEffect* SAOSampleFX;
	D3DXHANDLE CSZBuffer;
	D3DXHANDLE GBuffer0;
	D3DXHANDLE View;
	D3DXHANDLE SAOTech;
	D3DXHANDLE Radius;
	D3DXHANDLE Radius2;
	D3DXHANDLE Bias;
	D3DXHANDLE Intensity;
	D3DXHANDLE IntensityDivR6;
	D3DXHANDLE ProjScale;
	D3DXHANDLE Res;
	D3DXHANDLE FarPlane;


	ID3DXEffect* SAOBlurFX;
	D3DXHANDLE SAOBlurTech;
	D3DXHANDLE Source;
	D3DXHANDLE Axis;
	D3DXHANDLE gRes;

	UINT mHeight, mWidth;
private :
	float mRadius;
	float mRadius2;
	float mRadius6;
	float mBias;
	float mIntensity;
	float mProjScale;//0.5h/ tan 

};


#endif
