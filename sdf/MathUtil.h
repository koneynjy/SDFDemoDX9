#ifndef _MATHUTIL
#define _MATHUTIL

#include "Config.h"

#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.00002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_NORMALS_ARE_PARALLEL		(0.999845f)	/* Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
#define THRESH_NORMALS_ARE_ORTHOGONAL	(0.017455f)	/* Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */

#define DegreesToRadians(x) ((x) * (PI / 180.0f))

// template <typename T>
// inline  T Align(const T Ptr, int32 Alignment)
// {
// 	return (T)(((PTRINT)Ptr + Alignment - 1) & ~(Alignment - 1));
// }

namespace FMath{
	template< class T >
	static  FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	/** Returns 1, 0, or -1 depending on relation of T to 0 */
	template< class T >
	static  FORCEINLINE T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** Returns higher value in a generic way */
	template< class T >
	static  FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	/** Returns lower value in a generic way */
	template< class T >
	static  FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	/** Returns highest of 3 values */
	template< class T >
	static FORCEINLINE T Max3(const T A, const T B, const T C)
	{
		return Max(Max(A, B), C);
	}

	/** Returns lowest of 3 values */
	template< class T >
	static FORCEINLINE T Min3(const T A, const T B, const T C)
	{
		return Min(Min(A, B), C);
	}

	/** Multiples value by itself */
	template< class T >
	static FORCEINLINE T Square(const T A)
	{
		return A*A;
	}

	/** Clamps X to be between Min and Max, inclusive */
	template< class T >
	static FORCEINLINE T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	static FORCEINLINE int32 TruncToInt(float F)
	{
		return (int32)F;
	}

	/**
	* Converts a float to an integer value with truncation towards zero.
	* @param F		Floating point value to convert
	* @return		Truncated integer value.
	*/
	static  FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	/**
	* Converts a float to a nearest less or equal integer.
	* @param F		Floating point value to convert
	* @return		An integer less or equal to 'F'.
	*/
	static FORCEINLINE int32 FloorToInt(float F)
	{
		return TruncToInt(floorf(F));
	}

	/**
	* Converts a float to the nearest less or equal integer.
	* @param F		Floating point value to convert
	* @return		An integer less or equal to 'F'.
	*/
	static FORCEINLINE float FloorToFloat(float F)
	{
		return floorf(F);
	}

	/**
	* Converts a double to a less or equal integer.
	* @param F		Floating point value to convert
	* @return		The nearest integer value to 'F'.
	*/
	static FORCEINLINE double FloorToDouble(double F)
	{
		return floor(F);
	}

	/**
	* Converts a float to the nearest integer. Rounds up when the fraction is .5
	* @param F		Floating point value to convert
	* @return		The nearest integer to 'F'.
	*/
	static FORCEINLINE int32 RoundToInt(float F)
	{
		return FloorToInt(F + 0.5f);
	}

	/**
	* Converts a float to the nearest integer. Rounds up when the fraction is .5
	* @param F		Floating point value to convert
	* @return		The nearest integer to 'F'.
	*/
	static FORCEINLINE float RoundToFloat(float F)
	{
		return FloorToFloat(F + 0.5f);
	}

	/**
	* Converts a float to the nearest greater or equal integer.
	* @param F		Floating point value to convert
	* @return		An integer greater or equal to 'F'.
	*/
	static FORCEINLINE int32 CeilToInt(float F)
	{
		return TruncToInt(ceilf(F));
	}

	/**
	* Converts a float to the nearest greater or equal integer.
	* @param F		Floating point value to convert
	* @return		An integer greater or equal to 'F'.
	*/
	static FORCEINLINE float CeilToFloat(float F)
	{
		return ceilf(F);
	}

	/**
	* Returns signed fractional part of a float.
	* @param Value	Floating point value to convert
	* @return		A float between >=0 and < 1 for nonnegative input. A float between >= -1 and < 0 for negative input.
	*/
	static FORCEINLINE float Fractional(float Value)
	{
		return Value - TruncToFloat(Value);
	}

	/**
	* Returns the fractional part of a float.
	* @param Value	Floating point value to convert
	* @return		A float between >=0 and < 1.
	*/
	static FORCEINLINE float Frac(float Value)
	{
		return Value - FloorToFloat(Value);
	}

	static FORCEINLINE float Exp(float Value) { return expf(Value); }
	static FORCEINLINE float Loge(float Value) { return logf(Value); }
	static FORCEINLINE float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
	// 1.0 / Loge(2) = 1.4426950f
	static FORCEINLINE float Log2(float Value) { return Loge(Value) * 1.4426950f; }


	static FORCEINLINE float Fmod(float X, float Y) { return fmodf(X, Y); }
	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float Atan(float Value) { return atanf(Value); }
	static FORCEINLINE float Atan2(float Y, float X) { return atan2f(Y, X); }
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float Pow(float A, float B) { return powf(A, B); }

	static FORCEINLINE bool IsNaN(float A)
	{
		return ((*(uint32*)&A) & 0x7FFFFFFF) > 0x7F800000;
	}
	static FORCEINLINE bool IsFinite(float A)
	{
		return ((*(uint32*)&A) & 0x7F800000) != 0x7F800000;
	}
	static FORCEINLINE bool IsNegativeFloat(const float& F1)
	{
		return ((*(uint32*)&F1) >= (uint32)0x80000000); // Detects sign bit.
	}

	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (INV_PI*0.5f)*Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f*PI)*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign*p;
	}

	static FORCEINLINE float InvSqrt(float number){
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long *)&y;                       // evil floating point bit level hacking（对浮点数的邪恶位级hack）
		i = 0x5f375a86 - (i >> 1);               // what the fuck?（这他妈的是怎么回事？）
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration （第一次牛顿迭代）
		y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed（第二次迭代，可以删除）
		return y;
	}

	static FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	static FORCEINLINE float GridSnap(float Location, float Grid)
	{
		if (Grid == 0.f)	return Location;
		else
		{
			return FloorToFloat((Location + 0.5f*Grid) / Grid)*Grid;
		}
	}

	/** Returns a random integer between 0 and RAND_MAX, inclusive */
	static FORCEINLINE int32 Rand() { return rand(); }

	/** Seeds global random number functions Rand() and FRand() */
	static FORCEINLINE void RandInit(int32 Seed) { srand(Seed); }

	/** Returns a random float between 0 and 1, inclusive. */
	static FORCEINLINE float FRand() { return Rand() / (float)RAND_MAX; }

	/** Divides two integers and rounds up */
	template <class T>
	static FORCEINLINE T DivideAndRoundUp(T Dividend, T Divisor)
	{
		return (Dividend + Divisor - 1) / Divisor;
	}

	template <class T>
	static FORCEINLINE T DivideAndRoundDown(T Dividend, T Divisor)
	{
		return Dividend / Divisor;
	}
}



#endif // !_MATHUTIL
