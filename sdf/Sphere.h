#ifndef _SPHERE
#define _SPHERE

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "Config.h"
#include "Vector.h"


/**
* Implements a basic sphere.
*/
struct FSphere
{
public:

	/** Holds the sphere's center. */
	FVector Center;

	/** Holds the sphere's radius. */
	float W;

public:

	/** Default constructor (no initialization). */
	FSphere() { }

	/**
	* Creates and initializes a new sphere.
	*
	* @param int32 Passing int32 sets up zeroed sphere.
	*/
	FSphere(int32)
		: Center(0.0f, 0.0f, 0.0f)
		, W(0)
	{ }

	/**
	* Creates and initializes a new sphere with the specified parameters.
	*
	* @param InV Center of sphere.
	* @param InW Radius of sphere.
	*/
	FSphere(FVector InV, float InW)
		: Center(InV)
		, W(InW)
	{ }

	/**
	* Constructor.
	*
	* @param EForceInit Force Init Enum.
	*/
	explicit FORCEINLINE FSphere(EForceInit)
		: Center(ForceInit)
		, W(0.0f)
	{ }

	/**
	* Constructor.
	*
	* @param Pts Pointer to list of points this sphere must contain.
	* @param Count How many points are in the list.
	*/
	FSphere(const FVector* Pts, int32 Count);

public:

	/**
	* Check whether two spheres are the same within specified tolerance.
	*
	* @param Sphere The other sphere.
	* @param Tolerance Error Tolerance.
	* @return true if spheres are equal within specified tolerance, otherwise false.
	*/
	bool Equals(const FSphere& Sphere, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Center.Equals(Sphere.Center, Tolerance) && FMath::Abs(W - Sphere.W) < Tolerance;
	}

	/**
	* Check whether sphere is inside of another.
	*
	* @param Other The other sphere.
	* @param Tolerance Error Tolerance.
	* @return true if sphere is inside another, otherwise false.
	*/
	bool IsInside(const FSphere& Other, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		if (W > Other.W + Tolerance)
		{
			return false;
		}

		return (Center - Other.Center).SizeSquared() <= FMath::Square(Other.W + Tolerance - W);
	}


	/**
	* Checks whether the given location is inside this sphere.
	*
	* @param In The location to test for inside the bounding volume.
	* @return true if location is inside this volume.
	*/
	bool IsInside(const FVector& In, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return (Center - In).SizeSquared() <= FMath::Square(W + Tolerance);
	}


	/**
	* Test whether this sphere intersects another.
	*
	* @param  Other The other sphere.
	* @param  Tolerance Error tolerance.
	* @return true if spheres intersect, false otherwise.
	*/
	FORCEINLINE bool Intersects(const FSphere& Other, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return (Center - Other.Center).SizeSquared() <= FMath::Square(FMath::Max(0.f, Other.W + W + Tolerance));
	}

	/**
	* Get result of Transforming sphere by Matrix.
	*
	* @param M Matrix to transform by.
	* @return Result of transformation.
	*/

	float GetVolume() const;

	/**
	* Adds to this bounding box to include a new bounding volume.
	*
	* @param Other the bounding volume to increase the bounding volume to.
	* @return Reference to this bounding volume after resizing to include the other bounding volume.
	*/
	FSphere& operator+=(const FSphere& Other);

	/**
	* Gets the result of addition to this bounding volume.
	*
	* @param Other The other volume to add to this.
	* @return A new bounding volume.
	*/
	FSphere operator+(const FSphere& Other) const
	{
		return FSphere(*this) += Other;
	}
};


#endif