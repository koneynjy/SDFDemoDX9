//=============================================================================
// DrawableTex2D.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef DRAWABLE_TEX2D_H
#define DRAWABLE_TEX2D_H

#include "d3dUtil.h"

class DrawableTex2D
{
public:
	DrawableTex2D(UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport, bool autoGenMips);
	~DrawableTex2D();

	IDirect3DTexture9* d3dTex();
	IDirect3DSurface9* d3dSurface();
	IDirect3DSurface9* d3dSurfaceLod(int l){return mMipSurfaces[l];};
	void beginScene();
	void endScene();

	void onLostDevice();
	void onResetDevice();

private:
	// This class is not designed to be copied.
	DrawableTex2D(const DrawableTex2D& rhs);
	DrawableTex2D& operator=(const DrawableTex2D& rhs);

public:
	IDirect3DTexture9*    mTex;
	ID3DXRenderToSurface* mRTS;
	IDirect3DSurface9**   mMipSurfaces;

	UINT         mWidth;
	UINT         mHeight;
	UINT         mMipLevels;
	D3DFORMAT    mTexFormat;
	bool         mUseDepthBuffer;
	D3DFORMAT    mDepthFormat;
	D3DVIEWPORT9 mViewPort;
	bool         mAutoGenMips;
};

#endif // DRAWABLE_TEX2D_H