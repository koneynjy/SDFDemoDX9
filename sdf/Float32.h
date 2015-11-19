// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef _FLOAT32
#define _FLOAT32
#include "Config.h"
/**
* 32 bit float components
*/
struct FFloat32
{
public:

	union
	{
		struct
		{
			uint32	Mantissa : 23;
			uint32	Exponent : 8;
			uint32	Sign : 1;
		} Components;

		float	FloatValue;
	};

	/**
	* Constructor
	*
	* @param InValue value of the float.
	*/
	FFloat32(float InValue = 0.0f);
};


FORCEINLINE FFloat32::FFloat32(float InValue)
	: FloatValue(InValue)
{ }

#endif