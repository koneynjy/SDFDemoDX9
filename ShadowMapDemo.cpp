//=============================================================================
// ShadowMapDemo.cpp by Frank Luna (C) 2005 All Rights Reserved.
//
// Demonstrates shadow mapping.
//
// Controls: Use mouse to look and 'W', 'S', 'A', and 'D' keys to move.
//=============================================================================

#include "d3dApp.h"
#include "DirectInput.h"
#include <crtdbg.h>
#include "GfxStats.h"
#include <list>
#include "Camera.h"
#include "Sky.h"
#include "Vertex.h"
#include "DrawableTex2D.h"
#include "SDF.h"
#include "io.h"
#include "DeferredShading.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "SDFShadow.h"

struct SpotLight
{
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXVECTOR3 posW;
	D3DXVECTOR3 dirW;  
	float  spotPower;
};

class ShadowMapDemo : public D3DApp
{
public:
	ShadowMapDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~ShadowMapDemo();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();
	void drawShadowMap();
	void buildFX();
	void BuildModel();
	void LoadSponza();
	void BuildCullingVolume(XMFLOAT3 lightDir);
	void BuildQuadPlane();
	void BuildFullScreenQuad();
	void ForwardShading();
	void BuildGBuffer();
	void BuildHIZ();
	void DeferredShadingPass();
	void SDFShadowPass(UINT idx);
	void DebugTexture(IDirect3DTexture9* tex);
private:
	GfxStats* mGfxStats;

	DeferredShading *mDeferredShading;
	SDFShadow *mSDFShadow; 

	Sky* mSky;
	ID3DXMesh* mSceneMesh;
	D3DXMATRIX mSceneWorld;
	std::vector<Mtrl> mSceneMtrls;
	std::vector<IDirect3DTexture9*> mSceneTextures;

	ID3DXMesh* mCarMesh;
	D3DXMATRIX mCarWorld;
	std::vector<Mtrl> mCarMtrls;
	std::vector<IDirect3DTexture9*> mCarTextures;

	IDirect3DTexture9* mWhiteTex;
	DrawableTex2D* mShadowMap;

	D3DXMATRIX mLightVP;

	ID3DXEffect* mFX;
	D3DXHANDLE   mhBuildShadowMapTech;
	D3DXHANDLE   mhLightWVP;

	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhWorldInvTrans;
	D3DXHANDLE   mhEyePosW;
	D3DXHANDLE   mhWorld;
	D3DXHANDLE   mhTex;
	D3DXHANDLE   mhShadowMap;
	D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;

	ID3DXEffect* mHIZFX;
	D3DXHANDLE   mhHIZTech;
	D3DXHANDLE	 mhInfo;
	D3DXHANDLE   mhDepthMap;


	SpotLight mLight;
	Mtrl mWhite;
	vector<LPDIRECT3DVOLUMETEXTURE9> mObjSDFSRV;
	vector<SDFModel*> mObjSDF;
	vector<IDirect3DVertexBuffer9*> mObjModelVB;
	vector<IDirect3DIndexBuffer9*> mObjModelIB;
	vector<UINT> mObjModelCnt;
	vector<UINT> mObjModelVertexCnt;
	vector<D3DXMATRIX> mObjModelMat;
	vector<IDirect3DVertexBuffer9*> mObjCullingVolumeVB;
	vector<IDirect3DIndexBuffer9*> mObjCullingVolumeIB;
	vector<UINT> mObjCullingVolumeCnt;

	IDirect3DVertexBuffer9* mPlaneVB;
	IDirect3DIndexBuffer9* mPlaneIB;

	ID3DXEffect* mDebugTexFX;
	D3DXHANDLE   mhViewARGBTech;
	D3DXHANDLE   mhTexture;

	IDirect3DVertexBuffer9* mDebugTexVB;
	IDirect3DIndexBuffer9* mDebugTexIB;

	IDirect3DVertexBuffer9* mSponzaVB;
	IDirect3DIndexBuffer9* mSponzaIB;
	UINT mSponzaVertexCnt;
	UINT mSponzaIndexCnt;
	D3DXMATRIX mSponzaMat;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	// Construct camera before application, since the application uses the camera.
	Camera camera;
	gCamera = &camera;

	ShadowMapDemo app(hInstance, "Shadow Map Demo", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;

	DirectInput di(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;

    return gd3dApp->run();
}

ShadowMapDemo::ShadowMapDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP)
: D3DApp(hInstance, winCaption, devType, requestedVP)
{
	if(!checkDeviceCaps())
	{
		MessageBox(0, "checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	InitAllVertexDeclarations();

	mGfxStats = new GfxStats();
	mSky = new Sky("desertcube1024.dds", 10000.0f);
 
	//LoadXFile("BasicColumnScene.x", &mSceneMesh, mSceneMtrls, mSceneTextures);
	D3DXMatrixIdentity(&mSceneWorld);

	//LoadXFile("car.x", &mCarMesh, mCarMtrls, mCarTextures);
	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 1.5f, 1.5f, 1.5f);
	D3DXMatrixTranslation(&T, 6.0f, 3.5f, -3.0f);
	mCarWorld = S*T;

	//HR(D3DXCreateTextureFromFile(gd3dDevice, "whitetex.dds", &mWhiteTex));

	// Set some light properties; other properties are set in update function,
	// where they are animated.
	mLight.ambient   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLight.diffuse   = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	mLight.spec      = D3DXCOLOR(0.8f, 0.8f, 0.7f, 1.0f);
	mLight.spotPower = 32.0f;
	
	mWhite.ambient = D3DXCOLOR(0.3f, 0.4f, 0.5f, 1.0f);
	mWhite.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mWhite.spec = D3DXCOLOR(0.9f, 0.9f, 0.9f, 0.9f);
	mWhite.specPower = 16.0f;

	// Create shadow map.
	D3DVIEWPORT9 vp = {0, 0, 512, 512, 0.0f, 1.0f};
	mShadowMap = new DrawableTex2D(512, 512, 1, D3DFMT_R32F, true, D3DFMT_D24X8, vp, false);

	// Initialize camera.
	gCamera->pos().y = 20.0f;
	gCamera->pos().z = -100.0f;
	gCamera->pos() = D3DXVECTOR3(4350.0f, 300, -550.0f);
	gCamera->setSpeed(50.0f);

// 	mGfxStats->addVertices(mSceneMesh->GetNumVertices());
// 	mGfxStats->addTriangles(mSceneMesh->GetNumFaces());
// 
// 	mGfxStats->addVertices(mCarMesh->GetNumVertices());
// 	mGfxStats->addTriangles(mCarMesh->GetNumFaces());
// 
// 	mGfxStats->addVertices(mSky->getNumVertices());
// 	mGfxStats->addTriangles(mSky->getNumTriangles());

	buildFX();
	BuildModel();
	BuildFullScreenQuad();
	BuildQuadPlane();
	mDeferredShading = new DeferredShading(md3dPP.BackBufferWidth, md3dPP.BackBufferHeight);
	mSDFShadow = new SDFShadow(md3dPP.BackBufferWidth, md3dPP.BackBufferHeight);
	onResetDevice();
	mDeferredShading->InitQuad();
	
}

ShadowMapDemo::~ShadowMapDemo()
{
// 	delete mGfxStats;
// 	delete mSky;
// 	ReleaseCOM(mFX);
// 	ReleaseCOM(mWhiteTex);
// 	delete mShadowMap;

	//ReleaseCOM(mSceneMesh);
	//for(UINT i = 0; i < mSceneTextures.size(); ++i)
		//ReleaseCOM(mSceneTextures[i]);

	//ReleaseCOM(mCarMesh);
	//for(UINT i = 0; i < mCarTextures.size(); ++i)
		//ReleaseCOM(mCarTextures[i]);

	DestroyAllVertexDeclarations();
}

bool ShadowMapDemo::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(gd3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader version 2.0 support.
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
		return false;

	// Check for pixel shader version 2.0 support.
	if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
		return false;

	// Check render target support.  The adapter format can be either the display mode format
	// for windowed mode, or D3DFMT_X8R8G8B8 for fullscreen mode, so we need to test against
	// both.  We use D3DFMT_R32F as the render texture format and D3DFMT_D24X8 as the 
	// render texture depth format.
	D3DDISPLAYMODE mode;
	md3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	// Windowed.
	if(FAILED(md3dObject->CheckDeviceFormat(D3DADAPTER_DEFAULT, mDevType, mode.Format, 
		D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_R32F)))
		return false;
	if(FAILED(md3dObject->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, mDevType, mode.Format,
		D3DFMT_R32F, D3DFMT_D24X8)))
		return false;

	// Fullscreen.
	if(FAILED(md3dObject->CheckDeviceFormat(D3DADAPTER_DEFAULT, mDevType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_R32F)))
		return false;
	if(FAILED(md3dObject->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, mDevType, D3DFMT_X8R8G8B8,
		D3DFMT_R32F, D3DFMT_D24X8)))
		return false;


	return true;
}

void ShadowMapDemo::onLostDevice()
{
	mGfxStats->onLostDevice();
	mSky->onLostDevice();
	mShadowMap->onLostDevice();
	mDeferredShading->onLostDeivce();
	mSDFShadow->OnLostDevice();
	HR(mFX->OnLostDevice());
	HR(mDebugTexFX->OnLostDevice());
}

void ShadowMapDemo::onResetDevice()
{
	mGfxStats->onResetDevice();
	mSky->onResetDevice();
	mShadowMap->onResetDevice();
	mDeferredShading->onResetDevice();
	mSDFShadow->OnResetDevice();
	HR(mFX->OnResetDevice());
	HR(mDebugTexFX->OnResetDevice());

	// The aspect ratio depends on the backbuffer dimensions, which can 
	// possibly change after a reset.  So rebuild the projection matrix.
	float w = (float)md3dPP.BackBufferWidth;
	float h = (float)md3dPP.BackBufferHeight;
	gCamera->setLens(D3DX_PI * 0.25f, w/h, 1.0f, 2000.0f);
}

void ShadowMapDemo::updateScene(float dt)
{
	mGfxStats->update(dt);

	gDInput->poll();

	gCamera->update(dt, 0, 0);

	// Animate spot light by rotating it on y-axis with respect to time.
	D3DXMATRIX lightView;
	D3DXVECTOR3 lightPosW(125.0f, 50.0f, 0.0f);
	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);

	static float t = 0.0f;
	t += dt;
	if( t >= 2.0f*D3DX_PI )
		t = 0.0f;
	D3DXMATRIX Ry;
	D3DXMatrixRotationY(&Ry, t);
	D3DXVec3TransformCoord(&lightPosW, &lightPosW, &Ry);

	D3DXMatrixLookAtLH(&lightView, &lightPosW, &lightTargetW, &lightUpW);
	
	D3DXMATRIX lightLens;
	float lightFOV = D3DX_PI*0.25f;
	D3DXMatrixPerspectiveFovLH(&lightLens, lightFOV, 1.0f, 1.0f, 200.0f);

	mLightVP = lightView*lightLens;

	// Setup a spotlight corresponding to the projector.
	D3DXVECTOR3 lightDirW = lightTargetW - lightPosW;
	D3DXVec3Normalize(&lightDirW, &lightDirW);
	mLight.posW      = lightPosW;
	mLight.dirW      = lightDirW;
}

void ShadowMapDemo::drawScene()
{
	//ForwardShading();
	DeferredShadingPass();
}

void ShadowMapDemo::BuildFullScreenQuad()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<VertexPos> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].pos    = quad.Vertices[i].Position;
// 		vertices[i].normal = quad.Vertices[i].Normal;
// 		vertices[i].tex0   = quad.Vertices[i].TexC;
	}

	HR(gd3dDevice->CreateVertexBuffer(
		sizeof(VertexPos) * vertices.size(),
		0,
		D3DFVF_XYZ,
		D3DPOOL_MANAGED,
		&mDebugTexVB,
		NULL
		));
	void *tvb;
	HR(mDebugTexVB->Lock(0, sizeof(VertexPos) * vertices.size(), &tvb, 0));
	memcpy(tvb, &vertices[0], sizeof(VertexPos) * vertices.size());
	HR(mDebugTexVB->Unlock());
	std::vector<UINT> &Indices = quad.Indices;
	HR(gd3dDevice->CreateIndexBuffer(
		sizeof(UINT)* Indices.size(),
		0,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&mDebugTexIB,
		NULL
		));

	void *tib;
	HR(mDebugTexIB->Lock(0, sizeof(UINT)*  Indices.size(), &tib, 0));
	memcpy(tib, &Indices[0], sizeof(UINT)*  Indices.size());
	HR(mDebugTexIB->Unlock());
}

void ShadowMapDemo::ForwardShading()
{
	drawShadowMap();
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0));
	HR(gd3dDevice->BeginScene());

	HR(mFX->SetTechnique(mhTech));

	HR(mFX->SetValue(mhLight, &mLight, sizeof(SpotLight)));
	HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3)));
	HR(mFX->SetTexture(mhShadowMap, mShadowMap->d3dTex()));

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));

	// Draw Scene mesh.
	HR(mFX->SetMatrix(mhLightWVP, &(mSceneWorld*mLightVP)));
	HR(mFX->SetMatrix(mhWVP, &(mSceneWorld*gCamera->viewProj())));
	HR(mFX->SetMatrix(mhWorld, &mSceneWorld));	
	for(UINT j = 0; j < mSceneMtrls.size(); ++j)
	{
		HR(mFX->SetValue(mhMtrl, &mSceneMtrls[j], sizeof(Mtrl)));

		if(mSceneTextures[j] != 0)
		{
			HR(mFX->SetTexture(mhTex, mSceneTextures[j]));
		}
		else
		{
			HR(mFX->SetTexture(mhTex, mWhiteTex));
		}

		HR(mFX->CommitChanges());
		HR(mSceneMesh->DrawSubset(j));
	}

	// Draw car mesh.
	HR(mFX->SetMatrix(mhLightWVP, &(mCarWorld*mLightVP)));
	HR(mFX->SetMatrix(mhWVP, &(mCarWorld*gCamera->viewProj())));
	HR(mFX->SetMatrix(mhWorld, &mCarWorld));	
	for(UINT j = 0; j < mCarMtrls.size(); ++j)
	{
		HR(mFX->SetValue(mhMtrl, &mCarMtrls[j], sizeof(Mtrl)));

		if(mCarTextures[j] != 0)
		{
			HR(mFX->SetTexture(mhTex, mCarTextures[j]));
		}
		else
		{
			HR(mFX->SetTexture(mhTex, mWhiteTex));
		}

		HR(mFX->CommitChanges());
		HR(mCarMesh->DrawSubset(j));
	}


	//////////////////////////////////////////////////////////////////////
	gd3dDevice->SetVertexDeclaration(VertexPNT::Decl);

	for(int i = 50; i < 51; i++ )
	{
		HR(mFX->SetValue(mhMtrl, &mSceneMtrls[0], sizeof(Mtrl)));
		HR(mFX->SetMatrix(mhWVP, &(mObjModelMat[i]*gCamera->viewProj())));
		HR(mFX->SetMatrix(mhWorld, &mObjModelMat[i]));
		HR(mFX->CommitChanges());
		HR(gd3dDevice->SetStreamSource(0, mObjModelVB[i], 0, sizeof(VertexPNT)));
		HR(gd3dDevice->SetIndices(mObjModelIB[i]));


		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0,0,mObjModelVertexCnt[i],
			0,mObjModelCnt[i] / 3));
	}

	// 	D3DXMATRIX e;
	// 	D3DXMatrixIdentity(&e);
	// 	HR(mFX->SetValue(mhMtrl, &mSceneMtrls[0], sizeof(Mtrl)));
	// 	HR(mFX->SetMatrix(mhWVP, &gCamera->viewProj()));
	// 	HR(mFX->SetMatrix(mhWorld, &e));
	// 	HR(mFX->CommitChanges());
	// 	HR(gd3dDevice->SetStreamSource(0, mPlaneVB, 0, sizeof(VertexPNT)));
	// 	HR(gd3dDevice->SetIndices(mPlaneIB));
	// 
	// 
	// 	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
	// 		0,0,4,
	// 		0,2));

	///////////////////////////////////////////////////////////////////////////
	HR(mFX->EndPass());
	HR(mFX->End());

	mGfxStats->display();

	HR(gd3dDevice->EndScene());

	// Present the backbuffer.
	HR(gd3dDevice->Present(0, 0, 0, 0));

}

void ShadowMapDemo::drawShadowMap()
{

	HR(gd3dDevice->SetRenderTarget(0, mShadowMap->d3dSurface()));
	HR(gd3dDevice->SetViewport(&mShadowMap->mViewPort));
	HR(gd3dDevice->BeginScene());
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0));
 
	HR(mFX->SetTechnique(mhBuildShadowMapTech));
	
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));

	// Draw scene mesh.
	HR(mFX->SetMatrix(mhLightWVP, &(mSceneWorld*mLightVP)));
	HR(mFX->CommitChanges());
	for(UINT j = 0; j < mSceneMtrls.size(); ++j)
	{
		for(UINT j = 0; j < mSceneMtrls.size(); ++j)
		{
			HR(mSceneMesh->DrawSubset(j));
		}
	}

	// Draw car mesh.
	HR(mFX->SetMatrix(mhLightWVP, &(mCarWorld*mLightVP)));
	HR(mFX->CommitChanges());
	for(UINT j = 0; j < mCarMtrls.size(); ++j)
	{
		for(UINT j = 0; j < mCarMtrls.size(); ++j)
		{
			HR(mCarMesh->DrawSubset(j));
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());

	HR(gd3dDevice->EndScene());
	HR(gd3dDevice->SetRenderTarget(0, mBackBuffer));
	HR(gd3dDevice->SetDepthStencilSurface(mDepthStencilBuffer));
}

void ShadowMapDemo::buildFX()
{
	 // Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "FX/LightShadow.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech               = mFX->GetTechniqueByName("LightShadowTech");
	mhBuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	mhLightWVP           = mFX->GetParameterByName(0, "gLightWVP");
	mhWVP                = mFX->GetParameterByName(0, "gWVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");
	mhMtrl               = mFX->GetParameterByName(0, "gMtrl");
	mhLight              = mFX->GetParameterByName(0, "gLight");
	mhEyePosW            = mFX->GetParameterByName(0, "gEyePosW");
	mhTex                = mFX->GetParameterByName(0, "gTex");
	mhShadowMap          = mFX->GetParameterByName(0, "gShadowMap");

	errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "FX/DebugTex.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mDebugTexFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhViewARGBTech       = mDebugTexFX->GetTechniqueByName("ViewArgbTech");
	mhTexture			 = mDebugTexFX->GetParameterByName(0, "gTexture");

	errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "FX/hiz.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mHIZFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhHIZTech			 = mHIZFX->GetTechniqueByName("HIZTech");
	mhDepthMap			 = mHIZFX->GetParameterByName(0, "gDepthMap");
	mhInfo				 = mHIZFX->GetParameterByName(0, "gInfo");

}


void ShadowMapDemo::BuildModel()
{
	///////////
	string file_name = "D:/lod/proxy/";
	vector<CMesh> meshs;
	string path = file_name + "*.*";
	_finddata_t file;
	long lf;
	if ((lf = _findfirst(path.c_str(), &file)) == -1l)//_findfirst返回的是long型; long __cdecl _findfirst(const char *, struct _finddata_t *)
		return;
	else
	{
		while (_findnext(lf, &file) == 0)//int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1
		{
			if (file.attrib != _A_SUBDIR)
			{
				string name(file.name);
				if (name != ".." && name != "." && name.length() > 6 &&
					name.substr(name.length() - 6, 6) == ".model")  {
						CMesh cmesh;
						short x = 0, z = 0;
						sscanf(name.substr(0, 4).c_str(), "%hx", &x);
						sscanf(name.substr(4, 4).c_str(), "%hx", &z);
						XMFLOAT3 pos = XMFLOAT3(x * 100 + 50.0f, 0, z * 100 + 50.0f);
						cmesh.Init(file_name + name, "D:/", pos);
						XMFLOAT4X4 world;
						XMStoreFloat4x4(&world, XMMatrixTranslation(pos.x, pos.y, pos.z));
						SDFModel *sdf = new SDFModel(cmesh);
						meshs.push_back(std::move(cmesh));
						mObjSDF.push_back(sdf);
						//mObjModelMat.push_back(world);
						std::string dds = file_name + name.substr(0, 9) + ".dds";
						//std::wstring wdds(dds.length(), L' ');
						//std::copy(dds.begin(), dds.end(), wdds.begin());
						LPDIRECT3DVOLUMETEXTURE9 SDFMap = 0;
						HR(D3DXCreateVolumeTextureFromFile(gd3dDevice, dds.c_str(),&SDFMap));
						mObjSDFSRV.push_back(SDFMap);
				}
			}
		}
	}
	_findclose(lf);
	//cmesh.Init("D:/scene/common/zw/zwshu/slj_zwshu0020_wb.model", "D:/", XMFLOAT3(0, 0, 0));
	////////////////////////////////////////////SDF///////////////////////

	mObjModelVB.resize(meshs.size());
	mObjModelIB.resize(meshs.size());
	mObjModelCnt.resize(meshs.size());
	mObjModelMat.resize(meshs.size());
	mObjModelVertexCnt.resize(meshs.size());
	for (int i = 0; i < meshs.size(); i++)
	{
		CMesh& cmesh = meshs[i];
		INDEX_LIST &ib = cmesh.GetIndexList();
		MESH_VERTEX &vb = cmesh.GetMeshVertex();
		UINT count = ib.size();
		gd3dDevice->CreateVertexBuffer(
			sizeof(VertexPNT) * vb.size(),
			0,
			D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_TEX0,
			D3DPOOL_MANAGED,
			&mObjModelVB[i],
			NULL
			);
		void *tvb;
		HR(mObjModelVB[i]->Lock(0, sizeof(VertexPNT) * vb.size(), &tvb, 0));
		memcpy(tvb, &vb[0], sizeof(VertexPNT) * vb.size());
		HR(mObjModelVB[i]->Unlock());

		gd3dDevice->CreateIndexBuffer(
			sizeof(UINT)* count,
			0,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&mObjModelIB[i],
			NULL
			);

		void *tib;
		HR(mObjModelIB[i]->Lock(0, sizeof(UINT)* count, &tib, 0));
		memcpy(tib, &ib[0], sizeof(UINT)* count);
		HR(mObjModelIB[i]->Unlock());

		mObjModelVertexCnt[i] = vb.size();
		mObjModelCnt[i] = count;
		mObjModelMat[i] = *(D3DXMATRIX*)&cmesh.GetWorldTrans();
	}
}

void ShadowMapDemo::LoadSponza()
{
	
	string path = "D:/sponza.obj";
	CMesh cmesh;
	cmesh.Init(path, "", XMFLOAT3(0,0,0));
	INDEX_LIST &ib = cmesh.GetIndexList();
	MESH_VERTEX &vb = cmesh.GetMeshVertex();
	UINT count = ib.size();
	gd3dDevice->CreateVertexBuffer(
		sizeof(VertexPNT) * vb.size(),
		0,
		D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_TEX0,
		D3DPOOL_MANAGED,
		&mSponzaVB,
		NULL
		);
	void *tvb;
	HR(mSponzaVB->Lock(0, sizeof(VertexPNT) * vb.size(), &tvb, 0));
	memcpy(tvb, &vb[0], sizeof(VertexPNT) * vb.size());
	HR(mSponzaVB->Unlock());

	gd3dDevice->CreateIndexBuffer(
		sizeof(UINT)* count,
		0,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&mSponzaIB,
		NULL
		);

	void *tib;
	HR(mSponzaIB->Lock(0, sizeof(UINT)* count, &tib, 0));
	memcpy(tib, &ib[0], sizeof(UINT)* count);
	HR(mSponzaIB->Unlock());

	mSponzaVertexCnt = vb.size();
	mSponzaIndexCnt = count;
	mSponzaMat = *(D3DXMATRIX*)&cmesh.GetWorldTrans();

}

void ShadowMapDemo::BuildQuadPlane()
{

	vector<VertexPNT> vb;
	vector<USHORT> ib;
	VertexPNT origin;
	origin.pos = D3DXVECTOR3(0, 0, 0);
	origin.normal = D3DXVECTOR3(0, 1, 0);
	for (int i = 0; i < 4; i++)
	{
		vb.push_back(origin);
	}

	float length = 100000;
	vb[0].pos.x += length; vb[0].pos.z += length;
	vb[1].pos.x += length; vb[1].pos.z -= length;
	vb[2].pos.x -= length; vb[2].pos.z += length;
	vb[3].pos.x -= length; vb[3].pos.z -= length;

	vb[0].tex0.x = 1; vb[0].tex0.y = 1;
	vb[1].tex0.x = 1; vb[1].tex0.y = 0;
	vb[2].tex0.x = 0; vb[2].tex0.y = 1;
	vb[3].tex0.x = 0; vb[3].tex0.y = 0;

	ib.resize(6);
	ib[0] = 0; ib[1] = 1; ib[2] = 2;
	ib[3] = 2; ib[4] = 1; ib[5] = 3;


	HR(gd3dDevice->CreateVertexBuffer(
		sizeof(VertexPNT) * vb.size(),
		0,
		D3DFVF_XYZ| D3DFVF_NORMAL|D3DFVF_TEX0,
		D3DPOOL_MANAGED,
		&mPlaneVB,
		NULL
		));
	void *tvb;
	HR(mPlaneVB->Lock(0, sizeof(VertexPNT) * vb.size(), &tvb, 0));
	memcpy(tvb, &vb[0], sizeof(VertexPNT) * vb.size());
	HR(mPlaneVB->Unlock());

	HR(gd3dDevice->CreateIndexBuffer(
		sizeof(USHORT)* ib.size(),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&mPlaneIB,
		NULL
		));

	void *tib;
	HR(mPlaneIB->Lock(0, sizeof(USHORT)*  ib.size(), &tib, 0));
	memcpy(tib, &ib[0], sizeof(USHORT)*  ib.size());
	HR(mPlaneIB->Unlock());
}

void ShadowMapDemo::BuildGBuffer()
{
	mDeferredShading->GBufferBegin();
	ID3DXEffect* mFX = mDeferredShading->GBufferFX;

	gd3dDevice->SetVertexDeclaration(VertexPNT::Decl);

	for(int i = 0; i < 51; i++ )
	{
		HR(mFX->SetValue(mDeferredShading->mtrl, &mWhite, sizeof(Mtrl)));
		HR(mFX->SetMatrix(mDeferredShading->WorldInvTranspose, &MathHelper::InverseTranspose(mObjModelMat[i])));
		HR(mFX->SetMatrix(mDeferredShading->WorldViewProj, &(mObjModelMat[i]*gCamera->viewProj())));
		HR(mFX->CommitChanges());
		HR(gd3dDevice->SetStreamSource(0, mObjModelVB[i], 0, sizeof(VertexPNT)));
		HR(gd3dDevice->SetIndices(mObjModelIB[i]));


		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0,0,mObjModelVertexCnt[i],
			0,mObjModelCnt[i] / 3));
	}
// 
	D3DXMATRIX e;
	D3DXMatrixIdentity(&e);
	HR(mFX->SetValue(mDeferredShading->mtrl, &mWhite, sizeof(Mtrl)));
	HR(mFX->SetMatrix(mDeferredShading->WorldViewProj, &gCamera->viewProj()));
	HR(mFX->SetMatrix(mDeferredShading->WorldInvTranspose, &e));
	HR(mFX->CommitChanges());
	HR(gd3dDevice->SetStreamSource(0, mPlaneVB, 0, sizeof(VertexPNT)));
	HR(gd3dDevice->SetIndices(mPlaneIB));


	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,4,
		0,2));

	mDeferredShading->GBufferEnd();
}
/////////////////gen hi-z/////////////
void ShadowMapDemo::BuildHIZ()
{
	DrawableTex2D* ZMap = mDeferredShading->mDepthMap;
	UINT levelCount = ZMap->mMipLevels;
	D3DSURFACE_DESC  desc;
	HR(gd3dDevice->BeginScene());
	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
	HR(gd3dDevice->SetStreamSource(0, mDebugTexVB, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetIndices(mDebugTexIB));
	for(int i = 1; i < levelCount; i++)
	{
		ZMap->d3dTex()->GetLevelDesc(i - 1, &desc);
		D3DXVECTOR3 info = D3DXVECTOR3(desc.Width, desc.Height, i - 1);
		HR(gd3dDevice->SetRenderTarget(0, ZMap->d3dSurfaceLod(i)));
		//HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0));
		HR(mHIZFX->SetTechnique(mhHIZTech));
		HR(mHIZFX->SetTexture(mhDepthMap, ZMap->d3dTex()));
		HR(mHIZFX->SetValue(mhInfo, &info, sizeof(D3DXVECTOR3)));
		UINT numPasses = 0;
		HR(mHIZFX->Begin(&numPasses, 0));
		HR(mHIZFX->BeginPass(0));
		HR(mHIZFX->CommitChanges());

		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0,0,4,
			0,2));

		HR(mHIZFX->EndPass());
		HR(mHIZFX->End());
	}

	HR(gd3dDevice->EndScene());
}

void ShadowMapDemo::DebugTexture(IDirect3DTexture9* tex)
{
	HR(gd3dDevice->BeginScene());

	HR(mDebugTexFX->SetTechnique(mhViewARGBTech));
	HR(mDebugTexFX->SetTexture(mhTexture, tex));

	UINT numPasses = 0;
	HR(mDebugTexFX->Begin(&numPasses, 0));
	HR(mDebugTexFX->BeginPass(0));

	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
	HR(mDebugTexFX->CommitChanges());
	HR(gd3dDevice->SetStreamSource(0, mDebugTexVB, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetIndices(mDebugTexIB));
	HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,0,4,
		0,2));

	HR(mDebugTexFX->EndPass());
	HR(mDebugTexFX->End());
	mGfxStats->display();
	HR(gd3dDevice->EndScene());
	//mGfxStats->display();
}

void ShadowMapDemo::DeferredShadingPass()
{
	BuildGBuffer();
	//DebugTexture(mDeferredShading->mGBuffer1->d3dTex());
// 	for(UINT idx = 50; idx < 51; idx++)
// 		SDFShadowPass(idx);
	BuildHIZ();
	HR(gd3dDevice->SetRenderTarget(0,mBackBuffer));
	//DebugTexture(mDeferredShading->mGBuffer1->d3dTex());
	DebugTexture(mDeferredShading->mDepthMap->d3dTex());
//  	HR(gd3dDevice->BeginScene());	
//  	mDeferredShading->ShadingBegin();
//  	HR(mDeferredShading->DeferredShadingFX->SetValue(
//  		mDeferredShading->DirLights, &mLight, sizeof(SpotLight)));
//  	mDeferredShading->ShadingEnd();
 	//HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
 	//mSky->draw();
 	//HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
 	HR(gd3dDevice->EndScene());
	
	HR(gd3dDevice->Present(0, 0, 0, 0));
}

void ShadowMapDemo::SDFShadowPass(UINT idx)
{
	SDFModel* sdfModel = mObjSDF[idx];
	D3DXMATRIX worldMat = mObjModelMat[idx];
	D3DXVECTOR3 origin = *(D3DXVECTOR3*)&sdfModel->GetOrigin();
	D3DXVECTOR3 bounds = *(D3DXVECTOR3*)&sdfModel->GetBounds();
	D3DXVECTOR3 extends = bounds * 0.5f;
	D3DXMATRIX scaleMat ;
	D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX SDFToWord0 =  scaleMat * worldMat;
	mSDFShadow->GetCullingVolume(origin,extends,mLight.dirW,SDFToWord0);
	/////////////////////////////////////////////
	D3DXMATRIX SDFToWordInv0;
	float det;
	D3DXMatrixInverse(&SDFToWordInv0, &det, &SDFToWord0);
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans,-origin.x, -origin.y, -origin.z);
	SDFToWordInv0 *= trans;
	for(int i = 0; i < 3; i ++) SDFToWordInv0(3, i) += extends[i];
	D3DXMATRIX worldInvTranspose;
	D3DXMatrixInverse(&worldInvTranspose, &det, &scaleMat);
	D3DXVECTOR4 light;
	D3DXVec3Transform(&light, &mLight.dirW, &worldInvTranspose);

	HR(gd3dDevice->BeginScene());
	mSDFShadow->SDFShadowPassBegin();
	ID3DXEffect* mFX = mSDFShadow->SDFShadowFX;
	HR(mFX->SetValue(mSDFShadow->LightDir, &light, sizeof(D3DXVECTOR3)));
	HR(mFX->SetMatrix(mSDFShadow->SDFToWordInv0, &SDFToWordInv0));
	HR(mFX->SetValue(mSDFShadow->SDFBounds0, &bounds, sizeof(D3DXVECTOR3)));
	HR(mFX->SetFloat(mSDFShadow->SDFRes0, max(extends[0], max(extends[1], extends[2]))));
	HR(mFX->SetTexture (mSDFShadow->SDF0, mObjSDFSRV[idx]));
	HR(mFX->SetTexture (mSDFShadow->DepthMap, mDeferredShading->mDepthMap->d3dTex()));
	HR(mFX->SetTexture (mSDFShadow->GBuffer0, mDeferredShading->mGBuffer0->d3dTex()));
	HR(mFX->CommitChanges());
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for(UINT p = 0; p < numPasses; p++)
	{
		HR(mFX->BeginPass(p));
		mSDFShadow->SDFShadowPassDraw();
		HR(mFX->EndPass());
	}
	HR(mFX->End());
	HR(gd3dDevice->EndScene())

}