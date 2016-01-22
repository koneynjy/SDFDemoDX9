#ifndef _DEFERREDSHADING
#define _DEFERREDSHADING
#include "d3dUtil.h"
#include "Camera.h"
#include "DrawableTex2D.h"

class DeferredShading
{
public:
	DeferredShading(UINT width, UINT height);
	~DeferredShading();

	void GBufferBegin();
	void GBufferEnd();
	void ShadingBegin();
	void ShadingEnd();

	void onLostDeivce();
	void onResetDevice();
	void InitQuad();
	UINT mWidth;
	UINT mHeight;

	IDirect3DVertexBuffer9* mQuadVB;
	IDirect3DIndexBuffer9* mQuadIB;
	UINT vcount, icount;

	DrawableTex2D* mDepthMap;
	DrawableTex2D* mGBuffer0;
	DrawableTex2D* mGBuffer1;

	ID3DXEffect* GBufferFX;
	D3DXHANDLE GBufferTech;
	D3DXHANDLE WorldInvTranspose;
	D3DXHANDLE WorldViewProj;
	D3DXHANDLE FarClipDist;
	D3DXHANDLE mtrl;

	ID3DXEffect* DeferredShadingFX;
	D3DXHANDLE DeferredShadingTech;
	D3DXHANDLE DirLights;
	D3DXHANDLE ViewInv;
	D3DXHANDLE EyePosW;
	D3DXHANDLE amb;

	D3DXHANDLE gDepthMap;
	D3DXHANDLE gGBuffer0;
	D3DXHANDLE gGBuffer1;
	D3DXHANDLE gSDFShadow;
};

#endif // !_DEFERREDSHADING