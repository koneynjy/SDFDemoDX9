#ifndef _SAO
#define _SAO
#include "d3dUtil.h"
#include "Camera.h"
#include "DrawableTex2D.h"

class SAO
{
public:
	SAO();
	void OnLostDevice();
	void OnResetDevice();

	DrawableTex2D* mSAOMapFinal;
	DrawableTex2D* mSAOMapBlurred;
	DrawableTex2D* mSAOMapSampled;


	ID3DXEffect* SAOSampleTech;
	ID3DXEffect* SAOBlurTech;

};


#endif
