#ifndef _SDFSHADOW
#define _SDFSHADOW
#include "d3dUtil.h"
#include "Camera.h"

class SDFShadow
{
public:
	SDFShadow(UINT width, UINT height);
	~SDFShadow();

	void SetRenderTarget();
	void ClearShadow();

	void GetCullingVolume(XMFLOAT3&center, XMFLOAT3& extend, 
		XMFLOAT3& lightDir,XMFLOAT4X4& worldMat);
	UINT mWidth;
	UINT mHeight;

	D3DXHANDLE mSDFShadowSRV;
	D3DXHANDLE mSDFShadowRTV;

	D3DXHANDLE mPreSDFShadowSRV;
	D3DXHANDLE mPreSDFShadowRTV;

	D3DVIEWPORT9 mViewport;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	UINT mIndexCount;

};

#endif