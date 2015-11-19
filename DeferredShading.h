#ifndef _DEFERREDSHADING
#define _DEFERREDSHADING
#include "d3dUtil.h"
#include "Camera.h"

class DeferredShading
{
public:
	DeferredShading(UINT width, UINT height);
	~DeferredShading();

	void InitQuad(Camera &camera);
	void SetMRT();
	UINT mWidth;
	UINT mHeight;

};

#endif // !_DEFERREDSHADING