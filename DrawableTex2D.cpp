//=============================================================================
// DrawableTex2D.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "DrawableTex2D.h"
#include "d3dApp.h"

DrawableTex2D::DrawableTex2D(UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport,  bool autoGenMips)
: mTex(0), mRTS(0), mMipSurfaces(0), mWidth(width), mHeight(height), 
  mMipLevels(mipLevels), mTexFormat(texFormat), mUseDepthBuffer(useDepthBuffer),
  mDepthFormat(depthFormat), mViewPort(viewport), mAutoGenMips(autoGenMips)
{
}

DrawableTex2D::~DrawableTex2D()
{
	onLostDevice();
}

IDirect3DTexture9* DrawableTex2D::d3dTex()
{
	return mTex;
}

IDirect3DSurface9* DrawableTex2D::d3dSurface()
{
	return mMipSurfaces[0];
}

void DrawableTex2D::onLostDevice()
{
	ReleaseCOM(mTex);
	ReleaseCOM(mRTS);
	if(mMipSurfaces)
	{
		for(int i = 0; i < mMipLevels; i++)
			ReleaseCOM(mMipSurfaces[i]);
	}
}

void DrawableTex2D::onResetDevice()
{
	UINT usage = D3DUSAGE_RENDERTARGET;
	if(mAutoGenMips)
		usage |= D3DUSAGE_AUTOGENMIPMAP;

	HR(D3DXCreateTexture(gd3dDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex));
	HR(D3DXCreateRenderToSurface(gd3dDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS));
	if(mMipLevels == 0)
		mMipLevels = mTex->GetLevelCount();
	if(!mMipSurfaces) 
		mMipSurfaces = new IDirect3DSurface9*[mMipLevels];
	for(int i = 0; i < mMipLevels; i++)
	{
		HR(mTex->GetSurfaceLevel(i, &mMipSurfaces[i]));
	}
}

void DrawableTex2D::beginScene()
{
	//mRTS->BeginScene(mTopSurf, &mViewPort);
	//HR(gd3dDevice->SetRenderTarget(0, mTopSurf));
	HR(gd3dDevice->SetRenderTarget(0, mMipSurfaces[0]));
	HR(gd3dDevice->SetViewport(&mViewPort));
	HR(gd3dDevice->BeginScene());
}

void DrawableTex2D::endScene()
{
	//mRTS->EndScene(D3DX_FILTER_NONE);
	gd3dDevice->EndScene();
}