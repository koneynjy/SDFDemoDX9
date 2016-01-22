#ifndef _SDFSHADOW
#define _SDFSHADOW
#include "d3dUtil.h"
#include "Camera.h"
#include "DrawableTex2D.h"

class SDFShadow
{
public:
	SDFShadow(UINT width, UINT height);
	~SDFShadow();

	void SetRenderTarget();
	void ClearShadow();
	void GetCullingVolume(D3DXVECTOR3&center, D3DXVECTOR3& extend, 
		D3DXVECTOR3& lightDir,D3DXMATRIX& worldMat);
	void SDFShadowPassBegin();
	void SDFShadowPassDraw();
	void OnLostDevice();
	void OnResetDevice();
	UINT mWidth;
	UINT mHeight;

	DrawableTex2D* mSDFShadowMap;

	D3DVIEWPORT9 mViewport;

	ID3DXEffect* SDFShadowFX;
	D3DXHANDLE SDFShadowTech;
	D3DXHANDLE ViewProj;
	D3DXHANDLE EyePosW;
	D3DXHANDLE FarClipDist;
	D3DXHANDLE LightDir;
	D3DXHANDLE SDFToWordInv0;
	D3DXHANDLE SDFBounds0;
	D3DXHANDLE SDFRes0;
	D3DXHANDLE SDF0;
	D3DXHANDLE DepthMap;
	D3DXHANDLE GBuffer0;

	IDirect3DVertexBuffer9* mCullingVolumeVB;
	IDirect3DIndexBuffer9* mCullingVolumeIB;
	UINT mIndexCount;
	UINT mVertexCount;

};

#endif