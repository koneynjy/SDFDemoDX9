//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <Windows.h>
#include <d3dx9math.h>

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y)
	{
		float theta = 0.0f;

		// Quadrant I or IV
		if(x >= 0.0f) 
		{
			// If x = 0, then atanf(y/x) = +pi/2 if y > 0
			//                atanf(y/x) = -pi/2 if y < 0
			theta = atanf(y / x); // in [-pi/2, +pi/2]

			if(theta < 0.0f)
				theta += 2.0f*D3DX_PI; // in [0, 2*pi).
		}

		// Quadrant II or III
		else      
			theta = atanf(y/x) + D3DX_PI; // in [0, 2*pi).

		return theta;
	}

	static D3DXMATRIX InverseTranspose(D3DXMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		D3DXMATRIX A = M;
		A(3,0) = 0;
		A(3,1) = 0;
		A(3,2) = 0;
		A(3,3) = 1.0f;
		float det;
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv, &det, &A);
		D3DXMatrixTranspose(&A, &inv);
		return A;
	}


};

#endif // MATHHELPER_H