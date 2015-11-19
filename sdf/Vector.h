// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#ifndef _VECTOR
#define _VECTOR
#pragma once;
#include "Config.h"
#include "MathUtil.h"
/**
* A vector in 3-D space composed of components (X, Y, Z) with floating point precision.
*/
#pragma region VECTORDEC
struct FVector
{
public:

	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

	/** Vector's Z component. */
	float Z;

public:

	/** A zero vector (0,0,0) */
	static  const FVector ZeroVector;

	/** World up vector (0,0,1) */
	static  const FVector UpVector;

	/** Unreal forward vector (1,0,0) */
	static  const FVector ForwardVector;

	/** Unreal right vector (0,1,0) */
	static  const FVector RightVector;

public:

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		checkf(!ContainsNaN(), TEXT("FVector contains NaN: %s"), *ToString());
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const {}
#endif

	/** Default constructor (no initialization). */
	FORCEINLINE FVector();

	/**
	* Constructor initializing all components to a single float value.
	*
	* @param InF Value to set all components to.
	*/
	explicit FORCEINLINE FVector(float InF);

	/**
	* Constructor using initial values for each component.
	*
	* @param InX X Coordinate.
	* @param InY Y Coordinate.
	* @param InZ Z Coordinate.
	*/
	FORCEINLINE FVector(float InX, float InY, float InZ);

	FORCEINLINE FVector(const FVector4 &V4);
	/**
	* Constructor using the XYZ components from a 4D vector.
	*
	* @param V 4D Vector to copy from.
	*/
	//FORCEINLINE FVector(const FVector4& V);

	/**
	* Constructor which initializes all components to zero.
	*
	* @param EForceInit Force init enum
	*/
	explicit FORCEINLINE FVector(EForceInit);

#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
	/**
	* Copy another FVector into this one
	*
	* @param Other The other vector.
	* @return Reference to vector after copy.
	*/
	FORCEINLINE FVector& operator=(const FVector& Other);
#endif

	/**
	* Calculate cross product between this and another vector.
	*
	* @param V The other vector.
	* @return The cross product.
	*/
	FORCEINLINE FVector operator^(const FVector& V) const;

	/**
	* Calculate the cross product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The cross product.
	*/
	FORCEINLINE static FVector CrossProduct(const FVector& A, const FVector& B);

	/**
	* Calculate the dot product between this and another vector.
	*
	* @param V The other vector.
	* @return The dot product.
	*/
	FORCEINLINE float operator|(const FVector& V) const;

	/**
	* Calculate the dot product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The dot product.
	*/
	FORCEINLINE static float DotProduct(const FVector& A, const FVector& B);

	/**
	* Gets the result of component-wise addition of this and another vector.
	*
	* @param V The vector to add to this.
	* @return The result of vector addition.
	*/
	FORCEINLINE FVector operator+(const FVector& V) const;

	/**
	* Gets the result of component-wise subtraction of this by another vector.
	*
	* @param V The vector to subtract from this.
	* @return The result of vector subtraction.
	*/
	FORCEINLINE FVector operator-(const FVector& V) const;

	/**
	* Gets the result of subtracting from each component of the vector.
	*
	* @param Bias How much to subtract from each component.
	* @return The result of subtraction.
	*/
	FORCEINLINE FVector operator-(float Bias) const;

	/**
	* Gets the result of adding to each component of the vector.
	*
	* @param Bias How much to add to each component.
	* @return The result of addition.
	*/
	FORCEINLINE FVector operator+(float Bias) const;

	/**
	* Gets the result of scaling the vector (multiplying each component by a value).
	*
	* @param Scale What to multiply each component by.
	* @return The result of multiplication.
	*/
	FORCEINLINE FVector operator*(float Scale) const;

	/**
	* Gets the result of dividing each component of the vector by a value.
	*
	* @param Scale What to divide each component by.
	* @return The result of division.
	*/
	FVector operator/(float Scale) const;

	/**
	* Gets the result of component-wise multiplication of this vector by another.
	*
	* @param V The vector to multiply with.
	* @return The result of multiplication.
	*/
	FORCEINLINE FVector operator*(const FVector& V) const;

	/**
	* Gets the result of component-wise division of this vector by another.
	*
	* @param V The vector to divide by.
	* @return The result of division.
	*/
	FORCEINLINE FVector operator/(const FVector& V) const;

	// Binary comparison operators.

	/**
	* Check against another vector for equality.
	*
	* @param V The vector to check against.
	* @return true if the vectors are equal, false otherwise.
	*/
	bool operator==(const FVector& V) const;

	/**
	* Check against another vector for inequality.
	*
	* @param V The vector to check against.
	* @return true if the vectors are not equal, false otherwise.
	*/
	bool operator!=(const FVector& V) const;

	/**
	* Check against another vector for equality, within specified error limits.
	*
	* @param V The vector to check against.
	* @param Tolerance Error tolerance.
	* @return true if the vectors are equal within tolerance limits, false otherwise.
	*/
	bool Equals(const FVector& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether all components of this vector are the same, within a tolerance.
	*
	* @param Tolerance Error tolerance.
	* @return true if the vectors are equal within tolerance limits, false otherwise.
	*/
	bool AllComponentsEqual(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Get a negated copy of the vector.
	*
	* @return A negated copy of the vector.
	*/
	FORCEINLINE FVector operator-() const;

	/**
	* Adds another vector to this.
	* Uses component-wise addition.
	*
	* @param V Vector to add to this.
	* @return Copy of the vector after addition.
	*/
	FORCEINLINE FVector operator+=(const FVector& V);

	/**
	* Subtracts another vector from this.
	* Uses component-wise subtraction.
	*
	* @param V Vector to subtract from this.
	* @return Copy of the vector after subtraction.
	*/
	FORCEINLINE FVector operator-=(const FVector& V);

	/**
	* Scales the vector.
	*
	* @param Scale Amount to scale this vector by.
	* @return Copy of the vector after scaling.
	*/
	FORCEINLINE FVector operator*=(float Scale);

	/**
	* Divides the vector by a number.
	*
	* @param V What to divide this vector by.
	* @return Copy of the vector after division.
	*/
	FVector operator/=(float V);

	/**
	* Multiplies the vector with another vector, using component-wise multiplication.
	*
	* @param V What to multiply this vector with.
	* @return Copy of the vector after multiplication.
	*/
	FVector operator*=(const FVector& V);

	/**
	* Divides the vector by another vector, using component-wise division.
	*
	* @param V What to divide vector by.
	* @return Copy of the vector after division.
	*/
	FVector operator/=(const FVector& V);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return reference to component.
	*/
	float& operator[](int32 Index);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return Copy of the component.
	*/
	float operator[](int32 Index)const;

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	*
	* @return Reference to the specified component.
	*/
	float& Component(int32 Index);

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	* @return Copy of the specified component.
	*/
	float Component(int32 Index) const;

	// Simple functions.

	/**
	* Set the values of the vector directly.
	*
	* @param InX New X coordinate.
	* @param InY New Y coordinate.
	* @param InZ New Z coordinate.
	*/
	void Set(float InX, float InY, float InZ);

	/**
	* Get the maximum value of the vector's components.
	*
	* @return The maximum value of the vector's components.
	*/
	float GetMax() const;

	/**
	* Get the maximum absolute value of the vector's components.
	*
	* @return The maximum absolute value of the vector's components.
	*/
	float GetAbsMax() const;

	/**
	* Get the minimum value of the vector's components.
	*
	* @return The minimum value of the vector's components.
	*/
	float GetMin() const;

	/**
	* Get the minimum absolute value of the vector's components.
	*
	* @return The minimum absolute value of the vector's components.
	*/
	float GetAbsMin() const;

	/** Gets the component-wise min of two vectors. */
	FVector ComponentMin(const FVector& Other) const;

	/** Gets the component-wise max of two vectors. */
	FVector ComponentMax(const FVector& Other) const;

	/**
	* Get a copy of this vector with absolute value of each component.
	*
	* @return A copy of this vector with absolute value of each component.
	*/
	FVector GetAbs() const;

	/**
	* Get the length (magnitude) of this vector.
	*
	* @return The length of this vector.
	*/
	float Size() const;

	/**
	* Get the squared length of this vector.
	*
	* @return The squared length of this vector.
	*/
	float SizeSquared() const;

	/**
	* Get the length of the 2D components of this vector.
	*
	* @return The 2D length of this vector.
	*/
	float Size2D() const;

	/**
	* Get the squared length of the 2D components of this vector.
	*
	* @return The squared 2D length of this vector.
	*/
	float SizeSquared2D() const;

	/**
	* Checks whether vector is near to zero within a specified tolerance.
	*
	* @param Tolerance Error tolerance.
	* @return true if the vector is near to zero, false otherwise.
	*/
	bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether all components of the vector are exactly zero.
	*
	* @return true if the vector is exactly zero, false otherwise.
	*/
	bool IsZero() const;

	/**
	* Normalize this vector in-place if it is large enough, set it to (0,0,0) otherwise.
	*
	* @param Tolerance Minimum squared length of vector for normalization.
	* @return true if the vector was normalized correctly, false otherwise.
	*/
	bool Normalize(float Tolerance = SMALL_NUMBER);

	/**
	* Checks whether vector is normalized.
	*
	* @return true if Normalized, false otherwise.
	*/
	bool IsNormalized() const;

	/**
	* Util to convert this vector into a unit direction vector and its original length.
	*
	* @param OutDir Reference passed in to store unit direction vector.
	* @param OutLength Reference passed in to store length of the vector.
	*/
	void ToDirectionAndLength(FVector &OutDir, float &OutLength) const;

	/**
	* Get a copy of the vector as sign only.
	* Each component is set to +1 or -1, with the sign of zero treated as +1.
	*
	* @param A copy of the vector with each component set to +1 or -1
	*/
	FORCEINLINE FVector GetSignVector() const;

	/**
	* Projects 2D components of vector based on Z.
	*
	* @return Projected version of vector based on Z.
	*/
	FVector Projection() const;

	/**
	* Calculates normalized version of vector without checking for zero length.
	*
	* @return Normalized version of vector.
	* @see GetSafeNormal()
	*/
	FORCEINLINE FVector GetUnsafeNormal() const;

	/**
	* Gets a copy of this vector snapped to a grid.
	*
	* @param GridSz Grid dimension.
	* @return A copy of this vector snapped to a grid.
	* @see FMath::GridSnap()
	*/
	FVector GridSnap(const float& GridSz) const;

	/**
	* Get a copy of this vector, clamped inside of a cube.
	*
	* @param Radius Half size of the cube.
	* @return A copy of this vector, bound by cube.
	*/
	FVector BoundToCube(float Radius) const;

	/** Create a copy of this vector, with its magnitude clamped between Min and Max. */
	FVector GetClampedToSize(float Min, float Max) const;


	/** Create a copy of this vector, with the 2D magnitude clamped between Min and Max. Z is unchanged. */
	FVector GetClampedToSize2D(float Min, float Max) const;

	/** Create a copy of this vector, with its maximum magnitude clamped to MaxSize. */
	FVector GetClampedToMaxSize(float MaxSize) const;

	/** Create a copy of this vector, with the maximum 2D magnitude clamped to MaxSize. Z is unchanged. */
	FVector GetClampedToMaxSize2D(float MaxSize) const;

	/**
	* Add a vector to this and clamp the result in a cube.
	*
	* @param V Vector to add.
	* @param Radius Half size of the cube.
	*/
	void AddBounded(const FVector& V, float Radius = MAX_int16);

	/**
	* Gets the reciprocal of this vector, avoiding division by zero.
	* Zero components are set to BIG_NUMBER.
	*
	* @return Reciprocal of this vector.
	*/
	FVector Reciprocal() const;

	/**
	* Check whether X, Y and Z are nearly equal.
	*
	* @param Tolerance Specified Tolerance.
	* @return true if X == Y == Z within the specified tolerance.
	*/
	bool IsUniform(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Mirror a vector about a normal vector.
	*
	* @param MirrorNormal Normal vector to mirror about.
	* @return Mirrored vector.
	*/
	FVector MirrorByVector(const FVector& MirrorNormal) const;

	/**
	* Rotates around Axis (assumes Axis.Size() == 1).
	*
	* @param Angle Angle to rotate (in degrees).
	* @param Axis Axis to rotate around.
	* @return Rotated Vector.
	*/
	FVector RotateAngleAxis(const float AngleDeg, const FVector& Axis) const;

	/**
	* Gets a normalized copy of the vector, checking it is safe to do so based on the length.
	* Returns zero vector if vector length is too small to safely normalize.
	*
	* @param Tolerance Minimum squared vector length.
	* @return A normalized copy if safe, (0,0,0) otherwise.
	*/
	FVector GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

	/**
	* Gets a normalized copy of the 2D components of the vector, checking it is safe to do so. Z is set to zero.
	* Returns zero vector if vector length is too small to normalize.
	*
	* @param Tolerance Minimum squared vector length.
	* @return Normalized copy if safe, otherwise returns zero vector.
	*/
	FVector GetSafeNormal2D(float Tolerance = SMALL_NUMBER) const;

	/**
	* Returns the cosine of the angle between this vector and another projected onto the XY plane (no Z).
	*
	* @param B the other vector to find the 2D cosine of the angle with.
	* @return The cosine.
	*/
	FORCEINLINE float CosineAngle2D(FVector B) const;

	/**
	* Gets a copy of this vector projected onto the input vector.
	*
	* @param A	Vector to project onto, does not assume it is normalized.
	* @return Projected vector.
	*/
	FORCEINLINE FVector ProjectOnTo(const FVector& A) const;

	/**
	* Gets a copy of this vector projected onto the input vector, which is assumed to be unit length.
	*
	* @param  Normal Vector to project onto (assumed to be unit length).
	* @return Projected vector.
	*/
	FORCEINLINE FVector ProjectOnToNormal(const FVector& Normal) const;

	/**
	* Utility to check if there are any NaNs in this vector.
	*
	* @return true if there are any NaNs in this vector, false otherwise.
	*/
	bool ContainsNaN() const;

	/**
	* Check if the vector is of unit length, with specified tolerance.
	*
	* @param LengthSquaredTolerance Tolerance against squared length.
	* @return true if the vector is a unit vector within the specified tolerance.
	*/
	FORCEINLINE bool IsUnit(float LengthSquaredTolerance = HUGE_SMALL_NUMBER) const;

	/**
	* Convert a direction vector into a 'heading' angle.
	*
	* @return 'Heading' angle between +/-PI. 0 is pointing down +X.
	*/
	float HeadingAngle() const;

	/**
	* Create an orthonormal basis from a basis with at least two orthogonal vectors.
	* It may change the directions of the X and Y axes to make the basis orthogonal,
	* but it won't change the direction of the Z axis.
	* All axes will be normalized.
	*
	* @param XAxis The input basis' XAxis, and upon return the orthonormal basis' XAxis.
	* @param YAxis The input basis' YAxis, and upon return the orthonormal basis' YAxis.
	* @param ZAxis The input basis' ZAxis, and upon return the orthonormal basis' ZAxis.
	*/
	static  void CreateOrthonormalBasis(FVector& XAxis, FVector& YAxis, FVector& ZAxis);

	/**
	* Compare two points and see if they're the same, using a threshold.
	*
	* @param P First vector.
	* @param Q Second vector.
	* @return Whether points are the same within a threshold. Uses fast distance approximation (linear per-component distance).
	*/
	static bool PointsAreSame(const FVector &P, const FVector &Q);

	/**
	* Compare two points and see if they're within specified distance.
	*
	* @param Point1 First vector.
	* @param Point2 Second vector.
	* @param Dist Specified distance.
	* @return Whether two points are within the specified distance. Uses fast distance approximation (linear per-component distance).
	*/
	static bool PointsAreNear(const FVector &Point1, const FVector &Point2, float Dist);

	/**
	* Calculate the signed distance (in the direction of the normal) between a point and a plane.
	*
	* @param Point The Point we are checking.
	* @param PlaneBase The Base Point in the plane.
	* @param PlaneNormal The Normal of the plane (assumed to be unit length).
	* @return Signed distance between point and plane.
	*/
	static float PointPlaneDist(const FVector &Point, const FVector &PlaneBase, const FVector &PlaneNormal);


	/**
	* Calculate the projection of a point on the plane defined by counter-clockwise (CCW) points A,B,C.
	*
	* @param Point The point to project onto the plane
	* @param A 1st of three points in CCW order defining the plane
	* @param B 2nd of three points in CCW order defining the plane
	* @param C 3rd of three points in CCW order defining the plane
	* @return Projection of Point onto plane ABC
	*/
	static FVector PointPlaneProject(const FVector& Point, const FVector& A, const FVector& B, const FVector& C);

	/**
	* Calculate the projection of a point on the plane defined by PlaneBase and PlaneNormal.
	*
	* @param Point The point to project onto the plane
	* @param PlaneBase Point on the plane
	* @param PlaneNorm Normal of the plane (assumed to be unit length).
	* @return Projection of Point onto plane
	*/
	static FVector PointPlaneProject(const FVector& Point, const FVector& PlaneBase, const FVector& PlaneNormal);

	/**
	* Calculate the projection of a vector on the plane defined by PlaneNormal.
	*
	* @param  V The vector to project onto the plane.
	* @param  PlaneNormal Normal of the plane (assumed to be unit length).
	* @return Projection of V onto plane.
	*/
	static FVector VectorPlaneProject(const FVector& V, const FVector& PlaneNormal);

	/**
	* Euclidean distance between two points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The distance between two points.
	*/
	static FORCEINLINE float Dist(const FVector &V1, const FVector &V2);

	/**
	* Squared distance between two points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The squared distance between two points.
	*/
	static FORCEINLINE float DistSquared(const FVector &V1, const FVector &V2);

	/**
	* Compute pushout of a box from a plane.
	*
	* @param Normal The plane normal.
	* @param Size The size of the box.
	* @return Pushout required.
	*/
	static FORCEINLINE float BoxPushOut(const FVector& Normal, const FVector& Size);

	/**
	* Triple product of three vectors: X dot (Y cross Z).
	*
	* @param X The first vector.
	* @param Y The second vector.
	* @param Z The third vector.
	* @return The triple product: X dot (Y cross Z).
	*/
	static float Triple(const FVector& X, const FVector& Y, const FVector& Z);
};

#pragma endregion

#pragma region VECTOR4DEC

MS_ALIGN(16) struct FVector4
{
public:

	/** Holds the vector's X-component. */
	float X;

	/** Holds the vector's Y-component. */
	float Y;

	/** Holds the vector's Z-component. */
	float Z;

	/** Holds the vector's W-component. */
	float W;

public:

	/**
	* Creates and initializes a new vector from the specified components.
	*
	* @param InX X Coordinate.
	* @param InY Y Coordinate.
	* @param InZ Z Coordinate.
	* @param InW W Coordinate.
	*/
	explicit FVector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f);

	FVector4(const FVector& v);
	FVector4(const FVector4& v);

public:

	/**
	* Access a specific component of the vector.
	*
	* @param ComponentIndex The index of the component.
	* @return Reference to the desired component.
	*/
	FORCEINLINE float& operator[](int32 ComponentIndex);

	/**
	* Access a specific component of the vector.
	*
	* @param ComponentIndex The index of the component.
	* @return Copy of the desired component.
	*/
	FORCEINLINE float operator[](int32 ComponentIndex) const;

	// Unary operators.

	/**
	* Gets a negated copy of the vector.
	*
	* @return A negated copy of the vector.
	*/
	FORCEINLINE FVector4 operator-() const;

	/**
	* Gets the result of adding a vector to this.
	*
	* @param V The vector to add.
	* @return The result of vector addition.
	*/
	FORCEINLINE FVector4 operator+(const FVector4& V) const;

	/**
	* Adds another vector to this one.
	*
	* @param V The other vector to add.
	* @return Copy of the vector after addition.
	*/
	FORCEINLINE FVector4 operator+=(const FVector4& V);

	/**
	* Gets the result of subtracting a vector from this.
	*
	* @param V The vector to subtract.
	* @return The result of vector subtraction.
	*/
	FORCEINLINE FVector4 operator-(const FVector4& V) const;

	/**
	* Gets the result of scaling this vector.
	*
	* @param Scale The scaling factor.
	* @return The result of vector scaling.
	*/
	FORCEINLINE FVector4 operator*(float Scale) const;

	/**
	* Gets the result of dividing this vector.
	*
	* @param Scale What to divide by.
	* @return The result of division.
	*/
	FVector4 operator/(float Scale) const;

	/**
	* Gets the result of dividing this vector.
	*
	* @param V What to divide by.
	* @return The result of division.
	*/
	FVector4 operator/(const FVector4& V) const;

	/**
	* Gets the result of multiplying a vector with this.
	*
	* @param V The vector to multiply.
	* @return The result of vector multiplication.
	*/
	FVector4 operator*(const FVector4& V) const;

	/**
	* Gets the result of multiplying a vector with another Vector (component wise).
	*
	* @param V The vector to multiply.
	* @return The result of vector multiplication.
	*/
	FVector4 operator*=(const FVector4& V);

	/**
	* Gets the result of dividing a vector with another Vector (component wise).
	*
	* @param V The vector to divide with.
	* @return The result of vector multiplication.
	*/
	FVector4 operator/=(const FVector4& V);

	/**
	* Gets the result of scaling this vector.
	*
	* @param Scale The scaling factor.
	* @return The result of vector scaling.
	*/
	FVector4 operator*=(float S);

	FORCEINLINE float PlaneDot(const FVector4& V) const;

	/**
	* Calculates 3D Dot product of two 4D vectors.
	*
	* @param V1 The first vector.
	* @param V2 The second vector.
	* @return The 3D Dot product.
	*/
	friend FORCEINLINE float Dot3(const FVector4& V1, const FVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z;
	}

	/**
	* Calculates 4D Dot product.
	*
	* @param V1 The first vector.
	* @param V2 The second vector.
	* @return The 4D Dot Product.
	*/
	friend FORCEINLINE float Dot4(const FVector4& V1, const FVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z + V1.W*V2.W;
	}

	/**
	* Scales a vector.
	*
	* @param Scale The scaling factor.
	* @param V The vector to scale.
	* @return The result of scaling.
	*/
	friend FORCEINLINE FVector4 operator*(float Scale, const FVector4& V)
	{
		return V.operator*(Scale);
	}

	/**
	* Checks for equality against another vector.
	*
	* @param V The other vector.
	* @return true if the two vectors are the same, otherwise false.
	*/
	bool operator==(const FVector4& V) const;

	/**
	* Checks for inequality against another vector.
	*
	* @param V The other vector.
	* @return true if the two vectors are different, otherwise false.
	*/
	bool operator!=(const FVector4& V) const;

	/**
	* Calculate Cross product between this and another vector.
	*
	* @param V The other vector.
	* @return The Cross product.
	*/
	FVector4 operator^(const FVector4& V) const;

public:

	// Simple functions.

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component.
	* @return Reference to the component.
	*/
	float& Component(int32 Index);

	/**
	* Error tolerant comparison.
	*
	* @param V Vector to compare against.
	* @param Tolerance Error Tolerance.
	* @return true if the two vectors are equal within specified tolerance, otherwise false.
	*/
	bool Equals(const FVector4& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Check if the vector is of unit length, with specified tolerance.
	*
	* @param LengthSquaredTolerance Tolerance against squared length.
	* @return true if the vector is a unit vector within the specified tolerance.
	*/
	bool IsUnit3(float LengthSquaredTolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Returns a normalized copy of the vector if safe to normalize.
	*
	* @param Tolerance Minimum squared length of vector for normalization.
	* @return A normalized copy of the vector or a zero vector.
	*/
	FORCEINLINE FVector4 GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

	/**
	* Calculates normalized version of vector without checking if it is non-zero.
	*
	* @return Normalized version of vector.
	*/
	FORCEINLINE FVector4 GetUnsafeNormal3() const;

	/**
	* Set all of the vectors coordinates.
	*
	* @param InX New X Coordinate.
	* @param InY New Y Coordinate.
	* @param InZ New Z Coordinate.
	* @param InW New W Coordinate.
	*/
	FORCEINLINE void Set(float InX, float InY, float InZ, float InW);

	/**
	* Get the length of this vector not taking W component into account.
	*
	* @return The length of this vector.
	*/
	float Size3() const;

	/**
	* Get the squared length of this vector not taking W component into account.
	*
	* @return The squared length of this vector.
	*/
	float SizeSquared3() const;

	/** Utility to check if there are any NaNs in this vector. */
	bool ContainsNaN() const;

	/** Utility to check if all of the components of this vector are nearly zero given the tolerance. */
	bool IsNearlyZero3(float Tolerance = KINDA_SMALL_NUMBER) const;

	/** Reflect vector. */
	FVector4 Reflect3(const FVector4& Normal) const;

	/**
	* Find good arbitrary axis vectors to represent U and V axes of a plane,
	* given just the normal.
	*/
	void FindBestAxisVectors3(FVector4& Axis1, FVector4& Axis2) const;

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		checkf(!ContainsNaN(), TEXT("FVector contains NaN: %s"), *ToString());
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const { }
#endif

};

#pragma  endregion


#pragma region VECTORDEF
/* FVector inline functions
*****************************************************************************/

/**
* Multiplies a vector by a scaling factor.
*
* @param Scale Scaling factor.
* @param V Vector to scale.
* @return Result of multiplication.
*/
FORCEINLINE FVector operator*(float Scale, const FVector& V)
{
	return V.operator*(Scale);
}

/**
* Util to calculate distance from a point to a bounding box
*
* @param Mins 3D Point defining the lower values of the axis of the bound box
* @param Max 3D Point defining the lower values of the axis of the bound box
* @param Point 3D position of interest
* @return the distance from the Point to the bounding box.
*/
FORCEINLINE float ComputeSquaredDistanceFromBoxToPoint(const FVector& Mins, const FVector& Maxs, const FVector& Point)
{
	// Accumulates the distance as we iterate axis
	float DistSquared = 0.f;

	// Check each axis for min/max and add the distance accordingly
	// NOTE: Loop manually unrolled for > 2x speed up
	if (Point.X < Mins.X)
	{
		DistSquared += FMath::Square(Point.X - Mins.X);
	}
	else if (Point.X > Maxs.X)
	{
		DistSquared += FMath::Square(Point.X - Maxs.X);
	}

	if (Point.Y < Mins.Y)
	{
		DistSquared += FMath::Square(Point.Y - Mins.Y);
	}
	else if (Point.Y > Maxs.Y)
	{
		DistSquared += FMath::Square(Point.Y - Maxs.Y);
	}

	if (Point.Z < Mins.Z)
	{
		DistSquared += FMath::Square(Point.Z - Mins.Z);
	}
	else if (Point.Z > Maxs.Z)
	{
		DistSquared += FMath::Square(Point.Z - Maxs.Z);
	}

	return DistSquared;
}

/** A zero vector (0,0,0) */
const FVector FVector::ZeroVector = FVector(0);

/** World up vector (0,0,1) */
const FVector FVector::UpVector = FVector(0, 0, 1);

/** Unreal forward vector (1,0,0) */
const FVector FVector::ForwardVector = FVector(1, 0, 0);

/** Unreal right vector (0,1,0) */
const FVector FVector::RightVector = FVector(0, 1, 0);


inline FVector FVector::RotateAngleAxis(const float AngleDeg, const FVector& Axis) const
{
	float S, C;
	FMath::SinCos(&S, &C, DegreesToRadians(AngleDeg));

	const float XX = Axis.X * Axis.X;
	const float YY = Axis.Y * Axis.Y;
	const float ZZ = Axis.Z * Axis.Z;

	const float XY = Axis.X * Axis.Y;
	const float YZ = Axis.Y * Axis.Z;
	const float ZX = Axis.Z * Axis.X;

	const float XS = Axis.X * S;
	const float YS = Axis.Y * S;
	const float ZS = Axis.Z * S;

	const float OMC = 1.f - C;

	return FVector(
		(OMC * XX + C) * X + (OMC * XY - ZS) * Y + (OMC * ZX + YS) * Z,
		(OMC * XY + ZS) * X + (OMC * YY + C) * Y + (OMC * YZ - XS) * Z,
		(OMC * ZX - YS) * X + (OMC * YZ + XS) * Y + (OMC * ZZ + C) * Z
		);
}

inline bool FVector::PointsAreSame(const FVector &P, const FVector &Q)
{
	float Temp;
	Temp = P.X - Q.X;
	if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
	{
		Temp = P.Y - Q.Y;
		if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
		{
			Temp = P.Z - Q.Z;
			if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
			{
				return true;
			}
		}
	}
	return false;
}

inline bool FVector::PointsAreNear(const FVector &Point1, const FVector &Point2, float Dist)
{
	float Temp;
	Temp = (Point1.X - Point2.X); if (abs(Temp) >= Dist) return false;
	Temp = (Point1.Y - Point2.Y); if (abs(Temp) >= Dist) return false;
	Temp = (Point1.Z - Point2.Z); if (abs(Temp) >= Dist) return false;
	return true;
}

inline float FVector::PointPlaneDist
(
const FVector &Point,
const FVector &PlaneBase,
const FVector &PlaneNormal
)
{
	return (Point - PlaneBase) | PlaneNormal;
}


inline FVector FVector::PointPlaneProject(const FVector& Point, const FVector& PlaneBase, const FVector& PlaneNorm)
{
	//Find the distance of X from the plane
	//Add the distance back along the normal from the point
	return Point - FVector::PointPlaneDist(Point, PlaneBase, PlaneNorm) * PlaneNorm;
}

inline FVector FVector::VectorPlaneProject(const FVector& V, const FVector& PlaneNormal)
{
	return V - V.ProjectOnToNormal(PlaneNormal);
}


inline float FVector::Triple(const FVector& X, const FVector& Y, const FVector& Z)
{
	return
		((X.X * (Y.Y * Z.Z - Y.Z * Z.Y))
		+ (X.Y * (Y.Z * Z.X - Y.X * Z.Z))
		+ (X.Z * (Y.X * Z.Y - Y.Y * Z.X)));
}

FORCEINLINE FVector::FVector()
{}

FORCEINLINE FVector::FVector(float InF)
	: X(InF), Y(InF), Z(InF)
{
	DiagnosticCheckNaN();
}

FORCEINLINE FVector::FVector(float InX, float InY, float InZ)
	: X(InX), Y(InY), Z(InZ)
{
	DiagnosticCheckNaN();
}

FORCEINLINE FVector::FVector(const FVector4& V4)
	: X(V4.X), Y(V4.Y), Z(V4.Z)
{}

FORCEINLINE FVector::FVector(EForceInit)
	: X(0.0f), Y(0.0f), Z(0.0f)
{
	DiagnosticCheckNaN();
}

#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
FORCEINLINE FVector& FVector::operator=(const FVector& Other)
{
	this->X = Other.X;
	this->Y = Other.Y;
	this->Z = Other.Z;

	DiagnosticCheckNaN();

	return *this;
}
#endif

FORCEINLINE FVector FVector::operator^(const FVector& V) const
{
	return FVector
		(
		Y * V.Z - Z * V.Y,
		Z * V.X - X * V.Z,
		X * V.Y - Y * V.X
		);
}

FORCEINLINE FVector FVector::CrossProduct(const FVector& A, const FVector& B)
{
	return A ^ B;
}

FORCEINLINE float FVector::operator|(const FVector& V) const
{
	return X*V.X + Y*V.Y + Z*V.Z;
}

FORCEINLINE float FVector::DotProduct(const FVector& A, const FVector& B)
{
	return A | B;
}

FORCEINLINE FVector FVector::operator+(const FVector& V) const
{
	return FVector(X + V.X, Y + V.Y, Z + V.Z);
}

FORCEINLINE FVector FVector::operator-(const FVector& V) const
{
	return FVector(X - V.X, Y - V.Y, Z - V.Z);
}

FORCEINLINE FVector FVector::operator-(float Bias) const
{
	return FVector(X - Bias, Y - Bias, Z - Bias);
}

FORCEINLINE FVector FVector::operator+(float Bias) const
{
	return FVector(X + Bias, Y + Bias, Z + Bias);
}

FORCEINLINE FVector FVector::operator*(float Scale) const
{
	return FVector(X * Scale, Y * Scale, Z * Scale);
}

FORCEINLINE FVector FVector::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;
	return FVector(X * RScale, Y * RScale, Z * RScale);
}

FORCEINLINE FVector FVector::operator*(const FVector& V) const
{
	return FVector(X * V.X, Y * V.Y, Z * V.Z);
}

FORCEINLINE FVector FVector::operator/(const FVector& V) const
{
	return FVector(X / V.X, Y / V.Y, Z / V.Z);
}

FORCEINLINE bool FVector::operator==(const FVector& V) const
{
	return X == V.X && Y == V.Y && Z == V.Z;
}

FORCEINLINE bool FVector::operator!=(const FVector& V) const
{
	return X != V.X || Y != V.Y || Z != V.Z;
}

FORCEINLINE bool FVector::Equals(const FVector& V, float Tolerance) const
{
	return abs(X - V.X) <= Tolerance && abs(Y - V.Y) <= Tolerance && abs(Z - V.Z) <= Tolerance;
}

FORCEINLINE bool FVector::AllComponentsEqual(float Tolerance) const
{
	return abs(X - Y) < Tolerance && abs(X - Z) < Tolerance && abs(Y - Z) < Tolerance;
}


FORCEINLINE FVector FVector::operator-() const
{
	return FVector(-X, -Y, -Z);
}


FORCEINLINE FVector FVector::operator+=(const FVector& V)
{
	X += V.X; Y += V.Y; Z += V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE FVector FVector::operator-=(const FVector& V)
{
	X -= V.X; Y -= V.Y; Z -= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE FVector FVector::operator*=(float Scale)
{
	X *= Scale; Y *= Scale; Z *= Scale;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE FVector FVector::operator/=(float V)
{
	const float RV = 1.f / V;
	X *= RV; Y *= RV; Z *= RV;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE FVector FVector::operator*=(const FVector& V)
{
	X *= V.X; Y *= V.Y; Z *= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE FVector FVector::operator/=(const FVector& V)
{
	X /= V.X; Y /= V.Y; Z /= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE float& FVector::operator[](int32 Index)
{
	if (Index == 0)
	{
		return X;
	}
	else if (Index == 1)
	{
		return Y;
	}
	else
	{
		return Z;
	}
}

FORCEINLINE float FVector::operator[](int32 Index)const
{
	if (Index == 0)
	{
		return X;
	}
	else if (Index == 1)
	{
		return Y;
	}
	else
	{
		return Z;
	}
}

FORCEINLINE void FVector::Set(float InX, float InY, float InZ)
{
	X = InX;
	Y = InY;
	Z = InZ;
	DiagnosticCheckNaN();
}

FORCEINLINE float FVector::GetMax() const
{
	return FMath::Max(FMath::Max(X, Y), Z);
}

FORCEINLINE float FVector::GetAbsMax() const
{
	return FMath::Max(FMath::Max(abs(X), abs(Y)), abs(Z));
}

FORCEINLINE float FVector::GetMin() const
{
	return FMath::Min(FMath::Min(X, Y), Z);
}

FORCEINLINE float FVector::GetAbsMin() const
{
	return FMath::Min(FMath::Min(abs(X), abs(Y)), abs(Z));
}

FORCEINLINE FVector FVector::ComponentMin(const FVector& Other) const
{
	return FVector(FMath::Min(X, Other.X), FMath::Min(Y, Other.Y), FMath::Min(Z, Other.Z));
}

FORCEINLINE FVector FVector::ComponentMax(const FVector& Other) const
{
	return FVector(FMath::Max(X, Other.X), FMath::Max(Y, Other.Y), FMath::Max(Z, Other.Z));
}

FORCEINLINE FVector FVector::GetAbs() const
{
	return FVector(abs(X), abs(Y), abs(Z));
}

FORCEINLINE float FVector::Size() const
{
	return FMath::Sqrt(X*X + Y*Y + Z*Z);
}

FORCEINLINE float FVector::SizeSquared() const
{
	return X*X + Y*Y + Z*Z;
}

FORCEINLINE float FVector::Size2D() const
{
	return FMath::Sqrt(X*X + Y*Y);
}

FORCEINLINE float FVector::SizeSquared2D() const
{
	return X*X + Y*Y;
}

FORCEINLINE bool FVector::IsNearlyZero(float Tolerance) const
{
	return
		abs(X) < Tolerance
		&&	abs(Y) < Tolerance
		&&	abs(Z) < Tolerance;
}

FORCEINLINE bool FVector::IsZero() const
{
	return X == 0.f && Y == 0.f && Z == 0.f;
}

FORCEINLINE bool FVector::Normalize(float Tolerance)
{
	const float SquareSum = X*X + Y*Y + Z*Z;
	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);
		X *= Scale; Y *= Scale; Z *= Scale;
		return true;
	}
	return false;
}

FORCEINLINE bool FVector::IsNormalized() const
{
	return (abs(1.f - SizeSquared()) < THRESH_VECTOR_NORMALIZED);
}

FORCEINLINE void FVector::ToDirectionAndLength(FVector &OutDir, float &OutLength) const
{
	OutLength = Size();
	if (OutLength > SMALL_NUMBER)
	{
		float OneOverLength = 1.0f / OutLength;
		OutDir = FVector(X*OneOverLength, Y*OneOverLength,
			Z*OneOverLength);
	}
	else
	{
		OutDir = FVector::ZeroVector;
	}
}

FORCEINLINE FVector FVector::GetSignVector() const
{
	return FVector
		(
		FMath::FloatSelect(X, 1.f, -1.f),
		FMath::FloatSelect(Y, 1.f, -1.f),
		FMath::FloatSelect(Z, 1.f, -1.f)
		);
}

FORCEINLINE FVector FVector::Projection() const
{
	const float RZ = 1.f / Z;
	return FVector(X*RZ, Y*RZ, 1);
}

FORCEINLINE FVector FVector::GetUnsafeNormal() const
{
	const float Scale = FMath::InvSqrt(X*X + Y*Y + Z*Z);
	return FVector(X*Scale, Y*Scale, Z*Scale);
}

FORCEINLINE FVector FVector::GridSnap(const float& GridSz) const
{
	return FVector(FMath::GridSnap(X, GridSz), FMath::GridSnap(Y, GridSz), FMath::GridSnap(Z, GridSz));
}

FORCEINLINE FVector FVector::BoundToCube(float Radius) const
{
	return FVector
		(
		FMath::Clamp(X, -Radius, Radius),
		FMath::Clamp(Y, -Radius, Radius),
		FMath::Clamp(Z, -Radius, Radius)
		);
}

FORCEINLINE FVector FVector::GetClampedToSize(float Min, float Max) const
{
	float VecSize = Size();
	const FVector VecDir = (VecSize > SMALL_NUMBER) ? (*this / VecSize) : FVector::ZeroVector;

	VecSize = FMath::Clamp(VecSize, Min, Max);

	return VecSize * VecDir;
}

FORCEINLINE FVector FVector::GetClampedToSize2D(float Min, float Max) const
{
	float VecSize2D = Size2D();
	const FVector VecDir = (VecSize2D > SMALL_NUMBER) ? (*this / VecSize2D) : FVector::ZeroVector;

	VecSize2D = FMath::Clamp(VecSize2D, Min, Max);

	return FVector(VecSize2D * VecDir.X, VecSize2D * VecDir.Y, Z);
}


FORCEINLINE FVector FVector::GetClampedToMaxSize(float MaxSize) const
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	const float VSq = SizeSquared();
	if (VSq > FMath::Square(MaxSize))
	{
		const float Scale = MaxSize * FMath::InvSqrt(VSq);
		return FVector(X*Scale, Y*Scale, Z*Scale);
	}
	else
	{
		return *this;
	}
}

FORCEINLINE FVector FVector::GetClampedToMaxSize2D(float MaxSize) const
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return FVector(0.f, 0.f, Z);
	}

	const float VSq2D = SizeSquared2D();
	if (VSq2D > FMath::Square(MaxSize))
	{
		const float Scale = MaxSize * FMath::InvSqrt(VSq2D);
		return FVector(X*Scale, Y*Scale, Z);
	}
	else
	{
		return *this;
	}
}

FORCEINLINE void FVector::AddBounded(const FVector& V, float Radius)
{
	*this = (*this + V).BoundToCube(Radius);
}

FORCEINLINE float& FVector::Component(int32 Index)
{
	return (&X)[Index];
}

FORCEINLINE float FVector::Component(int32 Index) const
{
	return (&X)[Index];
}

FORCEINLINE FVector FVector::Reciprocal() const
{
	FVector RecVector;
	if (X != 0.f)
	{
		RecVector.X = 1.f / X;
	}
	else
	{
		RecVector.X = BIG_NUMBER;
	}
	if (Y != 0.f)
	{
		RecVector.Y = 1.f / Y;
	}
	else
	{
		RecVector.Y = BIG_NUMBER;
	}
	if (Z != 0.f)
	{
		RecVector.Z = 1.f / Z;
	}
	else
	{
		RecVector.Z = BIG_NUMBER;
	}

	return RecVector;
}

FORCEINLINE bool FVector::IsUniform(float Tolerance) const
{
	return (abs(X - Y) < Tolerance) && (abs(Y - Z) < Tolerance);
}

FORCEINLINE FVector FVector::MirrorByVector(const FVector& MirrorNormal) const
{
	return *this - MirrorNormal * (2.f * (*this | MirrorNormal));
}

FORCEINLINE FVector FVector::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y + Z*Z;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	}
	else if (SquareSum < Tolerance)
	{
		return FVector::ZeroVector;
	}
	const float Scale = FMath::InvSqrt(SquareSum);
	return FVector(X*Scale, Y*Scale, Z*Scale);
}

FORCEINLINE FVector FVector::GetSafeNormal2D(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		if (Z == 0.f)
		{
			return *this;
		}
		else
		{
			return FVector(X, Y, 0.f);
		}
	}
	else if (SquareSum < Tolerance)
	{
		return FVector::ZeroVector;
	}

	const float Scale = FMath::InvSqrt(SquareSum);
	return FVector(X*Scale, Y*Scale, 0.f);
}

FORCEINLINE float FVector::CosineAngle2D(FVector B) const
{
	FVector A(*this);
	A.Z = 0.0f;
	B.Z = 0.0f;
	A.Normalize();
	B.Normalize();
	return A | B;
}

FORCEINLINE FVector FVector::ProjectOnTo(const FVector& A) const
{
	return (A * ((*this | A) / (A | A)));
}

FORCEINLINE FVector FVector::ProjectOnToNormal(const FVector& Normal) const
{
	return (Normal * (*this | Normal));
}


FORCEINLINE bool FVector::ContainsNaN() const
{
	return (FMath::IsNaN(X) || !FMath::IsFinite(X) ||
		FMath::IsNaN(Y) || !FMath::IsFinite(Y) ||
		FMath::IsNaN(Z) || !FMath::IsFinite(Z));
}

FORCEINLINE bool FVector::IsUnit(float LengthSquaredTolerance) const
{
	return abs(1.0f - SizeSquared()) < LengthSquaredTolerance;
}

FORCEINLINE float FVector::HeadingAngle() const
{
	// Project Dir into Z plane.
	FVector PlaneDir = *this;
	PlaneDir.Z = 0.f;
	PlaneDir = PlaneDir.GetSafeNormal();

	float Angle = FMath::Acos(PlaneDir.X);

	if (PlaneDir.Y < 0.0f)
	{
		Angle *= -1.0f;
	}

	return Angle;
}



FORCEINLINE float FVector::Dist(const FVector &V1, const FVector &V2)
{
	return FMath::Sqrt(FMath::Square(V2.X - V1.X) + FMath::Square(V2.Y - V1.Y) + FMath::Square(V2.Z - V1.Z));
}

FORCEINLINE float FVector::DistSquared(const FVector &V1, const FVector &V2)
{
	return FMath::Square(V2.X - V1.X) + FMath::Square(V2.Y - V1.Y) + FMath::Square(V2.Z - V1.Z);
}

FORCEINLINE float FVector::BoxPushOut(const FVector& Normal, const FVector& Size)
{
	return abs(Normal.X*Size.X) + abs(Normal.Y*Size.Y) + abs(Normal.Z*Size.Z);
}

/** Component-wise clamp for FVector */
FORCEINLINE FVector ClampVector(const FVector& V, const FVector& Min, const FVector& Max)
{
	return FVector(
		FMath::Clamp(V.X, Min.X, Max.X),
		FMath::Clamp(V.Y, Min.Y, Max.Y),
		FMath::Clamp(V.Z, Min.Z, Max.Z)
		);
}

#pragma endregion

#pragma region VECTOR4DEC
/* FVector4 inline functions
*****************************************************************************/


FORCEINLINE FVector4::FVector4(float InX, float InY, float InZ, float InW)
	: X(InX), Y(InY), Z(InZ), W(InW)
{
	DiagnosticCheckNaN();
}

FORCEINLINE FVector4::FVector4(const FVector& V)
	: X(V.X), Y(V.Y), Z(V.Z), W(1.0f)
{}

FORCEINLINE FVector4::FVector4(const FVector4& V)
: X(V.X), Y(V.Y), Z(V.Z), W(V.W)
{}


FORCEINLINE float& FVector4::operator[](int32 ComponentIndex)
{
	return (&X)[ComponentIndex];
}


FORCEINLINE float FVector4::operator[](int32 ComponentIndex) const
{
	return (&X)[ComponentIndex];
}


FORCEINLINE void FVector4::Set(float InX, float InY, float InZ, float InW)
{
	X = InX;
	Y = InY;
	Z = InZ;
	W = InW;

	DiagnosticCheckNaN();
}


FORCEINLINE FVector4 FVector4::operator-() const
{
	return FVector4(-X, -Y, -Z, -W);
}


FORCEINLINE FVector4 FVector4::operator+(const FVector4& V) const
{
	return FVector4(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
}


FORCEINLINE FVector4 FVector4::operator+=(const FVector4& V)
{
	X += V.X; Y += V.Y; Z += V.Z; W += V.W;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE FVector4 FVector4::operator-(const FVector4& V) const
{
	return FVector4(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
}


FORCEINLINE FVector4 FVector4::operator*(float Scale) const
{
	return FVector4(X * Scale, Y * Scale, Z * Scale, W * Scale);
}


FORCEINLINE FVector4 FVector4::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;
	return FVector4(X * RScale, Y * RScale, Z * RScale, W * RScale);
}


FORCEINLINE FVector4 FVector4::operator*(const FVector4& V) const
{
	return FVector4(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
}


FORCEINLINE FVector4 FVector4::operator^(const FVector4& V) const
{
	return FVector4(
		Y * V.Z - Z * V.Y,
		Z * V.X - X * V.Z,
		X * V.Y - Y * V.X,
		0.0f
		);
}


FORCEINLINE float& FVector4::Component(int32 Index)
{
	return (&X)[Index];
}

FORCEINLINE float FVector4::PlaneDot(const FVector4& V) const
{
	return X*V.X + Y*V.Y + Z*V.Z - W;
}


FORCEINLINE bool FVector4::operator==(const FVector4& V) const
{
	return ((X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W));
}


FORCEINLINE bool FVector4::operator!=(const FVector4& V) const
{
	return ((X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W));
}


FORCEINLINE bool FVector4::Equals(const FVector4& V, float Tolerance) const
{
	return abs(X - V.X) < Tolerance && abs(Y - V.Y) < Tolerance && abs(Z - V.Z) < Tolerance && abs(W - V.W) < Tolerance;
}


FORCEINLINE FVector4 FVector4::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y + Z*Z;
	if (SquareSum > Tolerance)
	{
		const float Scale = 1.0f / sqrt(SquareSum);
		return FVector4(X*Scale, Y*Scale, Z*Scale, 0.0f);
	}
	return FVector4(0.f);
}



FORCEINLINE FVector4 FVector4::GetUnsafeNormal3() const
{
	const float Scale = 1.0f / sqrt(X*X + Y*Y + Z*Z);
	return FVector4(X*Scale, Y*Scale, Z*Scale, 0.0f);
}


FORCEINLINE float FVector4::Size3() const
{
	return sqrt(X*X + Y*Y + Z*Z);
}


FORCEINLINE float FVector4::SizeSquared3() const
{
	return X*X + Y*Y + Z*Z;
}


FORCEINLINE bool FVector4::IsUnit3(float LengthSquaredTolerance) const
{
	return abs(1.0f - SizeSquared3()) < LengthSquaredTolerance;
}


FORCEINLINE bool FVector4::ContainsNaN() const
{
	return (FMath::IsNaN(X) || !FMath::IsFinite(X) ||
		FMath::IsNaN(Y) || !FMath::IsFinite(Y) ||
		FMath::IsNaN(Z) || !FMath::IsFinite(Z) ||
		FMath::IsNaN(W) || !FMath::IsFinite(W));
}


FORCEINLINE bool FVector4::IsNearlyZero3(float Tolerance) const
{
	return
		abs(X) < Tolerance
		&&	abs(Y) < Tolerance
		&&	abs(Z) < Tolerance;
}


FORCEINLINE FVector4 FVector4::Reflect3(const FVector4& Normal) const
{
	return 2.0f * Dot3(*this, Normal) * Normal - *this;
}


FORCEINLINE void FVector4::FindBestAxisVectors3(FVector4& Axis1, FVector4& Axis2) const
{
	const float NX = abs(X);
	const float NY = abs(Y);
	const float NZ = abs(Z);

	// Find best basis vectors.
	if (NZ > NX && NZ > NY)	Axis1 = FVector4(1, 0, 0);
	else					Axis1 = FVector4(0, 0, 1);

	Axis1 = (Axis1 - *this * Dot3(Axis1, *this)).GetSafeNormal();
	Axis2 = Axis1 ^ *this;
}


FORCEINLINE FVector4 FVector4::operator*=(const FVector4& V)
{
	X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE FVector4 FVector4::operator/=(const FVector4& V)
{
	X /= V.X; Y /= V.Y; Z /= V.Z; W /= V.W;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE FVector4 FVector4::operator*=(float S)
{
	X *= S; Y *= S; Z *= S; W *= S;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE FVector4 FVector4::operator/(const FVector4& V) const
{
	return FVector4(X / V.X, Y / V.Y, Z / V.Z, W / V.W);
}
#pragma endregion


#pragma  region VECTOR2DEC
/**
* A vector in 2-D space composed of components (X, Y) with floating point precision.
*/
struct FVector2D
{
	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

public:

	/** Global 2D zero vector constant (0,0) */
	static const FVector2D ZeroVector;

	/** Global 2D unit vector constant (1,1) */
	static const FVector2D UnitVector;

public:

	/**
	* Default constructor (no initialization).
	*/
	FORCEINLINE FVector2D() { }

	/**
	* Constructor using initial values for each component.
	*
	* @param InX X coordinate.
	* @param InY Y coordinate.
	*/
	FORCEINLINE FVector2D(float InX, float InY);


	/**
	* Constructor which initializes all components to zero.
	*
	* @param EForceInit Force init enum
	*/
	explicit FORCEINLINE FVector2D(EForceInit);

	/**
	* Constructs a vector from an FVector.
	* Copies the X and Y components from the FVector.
	*
	* @param V Vector to copy from.
	*/
	explicit FORCEINLINE FVector2D(const FVector& V);

public:

	/**
	* Gets the result of adding two vectors together.
	*
	* @param V The other vector to add to this.
	* @return The result of adding the vectors together.
	*/
	FORCEINLINE FVector2D operator+(const FVector2D& V) const;

	/**
	* Gets the result of subtracting a vector from this one.
	*
	* @param V The other vector to subtract from this.
	* @return The result of the subtraction.
	*/
	FORCEINLINE FVector2D operator-(const FVector2D& V) const;

	/**
	* Gets the result of scaling the vector (multiplying each component by a value).
	*
	* @param Scale How much to scale the vector by.
	* @return The result of scaling this vector.
	*/
	FORCEINLINE FVector2D operator*(float Scale) const;

	/**
	* Gets the result of dividing each component of the vector by a value.
	*
	* @param Scale How much to divide the vector by.
	* @return The result of division on this vector.
	*/
	FVector2D operator/(float Scale) const;

	/**
	* Gets the result of this vector + float A.
	*
	* @param A		Float to add to each component.
	* @return		The result of this vector + float A.
	*/
	FORCEINLINE FVector2D operator+(float A) const;

	/**
	* Gets the result of subtracting from each component of the vector.
	*
	* @param A		Float to subtract from each component
	* @return		The result of this vector - float A.
	*/
	FORCEINLINE FVector2D operator-(float A) const;

	/**
	* Gets the result of component-wise multiplication of this vector by another.
	*
	* @param V The other vector to multiply this by.
	* @return The result of the multiplication.
	*/
	FORCEINLINE FVector2D operator*(const FVector2D& V) const;

	/**
	* Gets the result of component-wise division of this vector by another.
	*
	* @param V The other vector to divide this by.
	* @return The result of the division.
	*/
	FVector2D operator/(const FVector2D& V) const;

	/**
	* Calculates dot product of this vector and another.
	*
	* @param V The other vector.
	* @return The dot product.
	*/
	FORCEINLINE float operator|(const FVector2D& V) const;

	/**
	* Calculates cross product of this vector and another.
	*
	* @param V The other vector.
	* @return The cross product.
	*/
	FORCEINLINE float operator^(const FVector2D& V) const;

public:

	/**
	* Compares this vector against another for equality.
	*
	* @param V The vector to compare against.
	* @return true if the two vectors are equal, otherwise false.
	*/
	bool operator==(const FVector2D& V) const;

	/**
	* Compares this vector against another for inequality.
	*
	* @param V The vector to compare against.
	* @return true if the two vectors are not equal, otherwise false.
	*/
	bool operator!=(const FVector2D& V) const;

	/**
	* Checks whether both components of this vector are less than another.
	*
	* @param Other The vector to compare against.
	* @return true if this is the smaller vector, otherwise false.
	*/
	bool operator<(const FVector2D& Other) const;

	/**
	* Checks whether both components of this vector are greater than another.
	*
	* @param Other The vector to compare against.
	* @return true if this is the larger vector, otherwise false.
	*/
	bool operator>(const FVector2D& Other) const;

	/**
	* Checks whether both components of this vector are less than or equal to another.
	*
	* @param Other The vector to compare against.
	* @return true if this vector is less than or equal to the other vector, otherwise false.
	*/
	bool operator<=(const FVector2D& Other) const;

	/**
	* Checks whether both components of this vector are greater than or equal to another.
	*
	* @param Other The vector to compare against.
	* @return true if this vector is greater than or equal to the other vector, otherwise false.
	*/
	bool operator>=(const FVector2D& Other) const;

	/**
	* Gets a negated copy of the vector.
	*
	* @return A negated copy of the vector.
	*/
	FORCEINLINE FVector2D operator-() const;

	/**
	* Adds another vector to this.
	*
	* @param V The other vector to add.
	* @return Copy of the vector after addition.
	*/
	FORCEINLINE FVector2D operator+=(const FVector2D& V);

	/**
	* Subtracts another vector from this.
	*
	* @param V The other vector to subtract.
	* @return Copy of the vector after subtraction.
	*/
	FORCEINLINE FVector2D operator-=(const FVector2D& V);

	/**
	* Scales this vector.
	*
	* @param Scale The scale to multiply vector by.
	* @return Copy of the vector after scaling.
	*/
	FORCEINLINE FVector2D operator*=(float Scale);

	/**
	* Divides this vector.
	*
	* @param V What to divide vector by.
	* @return Copy of the vector after division.
	*/
	FVector2D operator/=(float V);

	/**
	* Multiplies this vector with another vector, using component-wise multiplication.
	*
	* @param V The vector to multiply with.
	* @return Copy of the vector after multiplication.
	*/
	FVector2D operator*=(const FVector2D& V);

	/**
	* Divides this vector by another vector, using component-wise division.
	*
	* @param V The vector to divide by.
	* @return Copy of the vector after division.
	*/
	FVector2D operator/=(const FVector2D& V);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return reference to component.
	*/
	float& operator[](int32 Index);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return copy of component value.
	*/
	float operator[](int32 Index) const;

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	* @return Reference to the specified component.
	*/
	float& Component(int32 Index);

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	* @return Copy of the specified component.
	*/
	float Component(int32 Index) const;

public:

	/**
	* Calculates the dot product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The dot product.
	*/
	FORCEINLINE static float DotProduct(const FVector2D& A, const FVector2D& B);

	/**
	* Squared distance between two 2D points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The squared distance between two 2D points.
	*/
	FORCEINLINE static float DistSquared(const FVector2D& V1, const FVector2D& V2);

	/**
	* Distance between two 2D points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The squared distance between two 2D points.
	*/
	FORCEINLINE static float Distance(const FVector2D& V1, const FVector2D& V2);

	/**
	* Calculate the cross product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The cross product.
	*/
	FORCEINLINE static float CrossProduct(const FVector2D& A, const FVector2D& B);

	/**
	* Checks for equality with error-tolerant comparison.
	*
	* @param V The vector to compare.
	* @param Tolerance Error tolerance.
	* @return true if the vectors are equal within specified tolerance, otherwise false.
	*/
	bool Equals(const FVector2D& V, float Tolerance) const;

	/**
	* Set the values of the vector directly.
	*
	* @param InX New X coordinate.
	* @param InY New Y coordinate.
	*/
	void Set(float InX, float InY);

	/**
	* Get the maximum value of the vector's components.
	*
	* @return The maximum value of the vector's components.
	*/
	float GetMax() const;

	/**
	* Get the maximum absolute value of the vector's components.
	*
	* @return The maximum absolute value of the vector's components.
	*/
	float GetAbsMax() const;

	/**
	* Get the minimum value of the vector's components.
	*
	* @return The minimum value of the vector's components.
	*/
	float GetMin() const;

	/**
	* Get the length (magnitude) of this vector.
	*
	* @return The length of this vector.
	*/
	float Size() const;

	/**
	* Get the squared length of this vector.
	*
	* @return The squared length of this vector.
	*/
	float SizeSquared() const;

	/**
	* Rotates around axis (0,0,1)
	*
	* @param AngleDeg Angle to rotate (in degrees)
	* @return Rotated Vector
	*/
	FVector2D GetRotated(float AngleDeg) const;

	/**
	* Gets a normalized copy of the vector, checking it is safe to do so based on the length.
	* Returns zero vector if vector length is too small to safely normalize.
	*
	* @param Tolerance Minimum squared length of vector for normalization.
	* @return A normalized copy of the vector if safe, (0,0) otherwise.
	*/
	FVector2D GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

	/**
	* Normalize this vector in-place if it is large enough, set it to (0,0) otherwise.
	*
	* @param Tolerance Minimum squared length of vector for normalization.
	* @see GetSafeNormal()
	*/
	void Normalize(float Tolerance = SMALL_NUMBER);

	/**
	* Checks whether vector is near to zero within a specified tolerance.
	*
	* @param Tolerance Error tolerance.
	* @return true if vector is in tolerance to zero, otherwise false.
	*/
	bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether all components of the vector are exactly zero.
	*
	* @return true if vector is exactly zero, otherwise false.
	*/
	bool IsZero() const;

	/**
	* Creates a copy of this vector with both axes clamped to the given range.
	* @return New vector with clamped axes.
	*/
	FVector2D ClampAxes(float MinAxisVal, float MaxAxisVal) const;

	/**
	* Get a copy of the vector as sign only.
	* Each component is set to +1 or -1, with the sign of zero treated as +1.
	*
	* @param A copy of the vector with each component set to +1 or -1
	*/
	FORCEINLINE FVector2D GetSignVector() const;

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		checkf(!ContainsNaN(), TEXT("FVector contains NaN: %s"), *ToString());
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const {}
#endif

	/**
	* Utility to check if there are any NaNs in this vector.
	*
	* @return true if there are any NaNs in this vector, otherwise false.
	*/
	FORCEINLINE bool ContainsNaN() const
	{
		return (FMath::IsNaN(X) || !FMath::IsFinite(X) ||
			FMath::IsNaN(Y) || !FMath::IsFinite(Y));
	}

	/** Converts spherical coordinates on the unit sphere into a Cartesian unit length vector. */
	inline FVector SphericalToUnitCartesian() const;
};


/* FVector2D inline functions
*****************************************************************************/

FORCEINLINE FVector2D operator*(float Scale, const FVector2D& V)
{
	return V.operator*(Scale);
}


FORCEINLINE FVector2D::FVector2D(float InX, float InY)
: X(InX), Y(InY)
{ }




FORCEINLINE FVector2D::FVector2D(EForceInit)
: X(0), Y(0)
{
}


FORCEINLINE FVector2D FVector2D::operator+(const FVector2D& V) const
{
	return FVector2D(X + V.X, Y + V.Y);
}


FORCEINLINE FVector2D FVector2D::operator-(const FVector2D& V) const
{
	return FVector2D(X - V.X, Y - V.Y);
}


FORCEINLINE FVector2D FVector2D::operator*(float Scale) const
{
	return FVector2D(X * Scale, Y * Scale);
}


FORCEINLINE FVector2D FVector2D::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;
	return FVector2D(X * RScale, Y * RScale);
}


FORCEINLINE FVector2D FVector2D::operator+(float A) const
{
	return FVector2D(X + A, Y + A);
}


FORCEINLINE FVector2D FVector2D::operator-(float A) const
{
	return FVector2D(X - A, Y - A);
}


FORCEINLINE FVector2D FVector2D::operator*(const FVector2D& V) const
{
	return FVector2D(X * V.X, Y * V.Y);
}


FORCEINLINE FVector2D FVector2D::operator/(const FVector2D& V) const
{
	return FVector2D(X / V.X, Y / V.Y);
}


FORCEINLINE float FVector2D::operator|(const FVector2D& V) const
{
	return X*V.X + Y*V.Y;
}


FORCEINLINE float FVector2D::operator^(const FVector2D& V) const
{
	return X*V.Y - Y*V.X;
}


FORCEINLINE float FVector2D::DotProduct(const FVector2D& A, const FVector2D& B)
{
	return A | B;
}


FORCEINLINE float FVector2D::DistSquared(const FVector2D &V1, const FVector2D &V2)
{
	return FMath::Square(V2.X - V1.X) + FMath::Square(V2.Y - V1.Y);
}


FORCEINLINE float FVector2D::Distance(const FVector2D& V1, const FVector2D& V2)
{
	return FMath::Sqrt(FVector2D::DistSquared(V1, V2));
}


FORCEINLINE float FVector2D::CrossProduct(const FVector2D& A, const FVector2D& B)
{
	return A ^ B;
}


FORCEINLINE bool FVector2D::operator==(const FVector2D& V) const
{
	return X == V.X && Y == V.Y;
}


FORCEINLINE bool FVector2D::operator!=(const FVector2D& V) const
{
	return X != V.X || Y != V.Y;
}


FORCEINLINE bool FVector2D::operator<(const FVector2D& Other) const
{
	return X < Other.X && Y < Other.Y;
}


FORCEINLINE bool FVector2D::operator>(const FVector2D& Other) const
{
	return X > Other.X && Y > Other.Y;
}


FORCEINLINE bool FVector2D::operator<=(const FVector2D& Other) const
{
	return X <= Other.X && Y <= Other.Y;
}


FORCEINLINE bool FVector2D::operator>=(const FVector2D& Other) const
{
	return X >= Other.X && Y >= Other.Y;
}


FORCEINLINE bool FVector2D::Equals(const FVector2D& V, float Tolerance) const
{
	return FMath::Abs(X - V.X) < Tolerance && FMath::Abs(Y - V.Y) < Tolerance;
}


FORCEINLINE FVector2D FVector2D::operator-() const
{
	return FVector2D(-X, -Y);
}


FORCEINLINE FVector2D FVector2D::operator+=(const FVector2D& V)
{
	X += V.X; Y += V.Y;
	return *this;
}


FORCEINLINE FVector2D FVector2D::operator-=(const FVector2D& V)
{
	X -= V.X; Y -= V.Y;
	return *this;
}


FORCEINLINE FVector2D FVector2D::operator*=(float Scale)
{
	X *= Scale; Y *= Scale;
	return *this;
}


FORCEINLINE FVector2D FVector2D::operator/=(float V)
{
	const float RV = 1.f / V;
	X *= RV; Y *= RV;
	return *this;
}


FORCEINLINE FVector2D FVector2D::operator*=(const FVector2D& V)
{
	X *= V.X; Y *= V.Y;
	return *this;
}


FORCEINLINE FVector2D FVector2D::operator/=(const FVector2D& V)
{
	X /= V.X; Y /= V.Y;
	return *this;
}


FORCEINLINE float& FVector2D::operator[](int32 Index)
{
	//assert(Index >= 0 && Index<2);
	return ((Index == 0) ? X : Y);
}


FORCEINLINE float FVector2D::operator[](int32 Index) const
{
	//check(Index >= 0 && Index<2);
	return ((Index == 0) ? X : Y);
}


FORCEINLINE void FVector2D::Set(float InX, float InY)
{
	X = InX;
	Y = InY;
}


FORCEINLINE float FVector2D::GetMax() const
{
	return FMath::Max(X, Y);
}


FORCEINLINE float FVector2D::GetAbsMax() const
{
	return FMath::Max(FMath::Abs(X), FMath::Abs(Y));
}


FORCEINLINE float FVector2D::GetMin() const
{
	return FMath::Min(X, Y);
}


FORCEINLINE float FVector2D::Size() const
{
	return FMath::Sqrt(X*X + Y*Y);
}


FORCEINLINE float FVector2D::SizeSquared() const
{
	return X*X + Y*Y;
}

FORCEINLINE FVector2D FVector2D::GetRotated(const float AngleDeg) const
{
	// Based on FVector::RotateAngleAxis with Axis(0,0,1)

	float S, C;
	FMath::SinCos(&S, &C, DegreesToRadians(AngleDeg));

	const float OMC = 1.0f - C;

	return FVector2D(
		C * X - S * Y,
		S * X + C * Y);
}

FORCEINLINE FVector2D FVector2D::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y;
	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);
		return FVector2D(X*Scale, Y*Scale);
	}
	return FVector2D(0.f, 0.f);
}


FORCEINLINE void FVector2D::Normalize(float Tolerance)
{
	const float SquareSum = X*X + Y*Y;
	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);
		X *= Scale;
		Y *= Scale;
		return;
	}
	X = 0.0f;
	Y = 0.0f;
}


FORCEINLINE bool FVector2D::IsNearlyZero(float Tolerance) const
{
	return	FMath::Abs(X)<Tolerance
		&&	FMath::Abs(Y)<Tolerance;
}


FORCEINLINE bool FVector2D::IsZero() const
{
	return X == 0.f && Y == 0.f;
}


FORCEINLINE float& FVector2D::Component(int32 Index)
{
	return (&X)[Index];
}


FORCEINLINE float FVector2D::Component(int32 Index) const
{
	return (&X)[Index];
}



FORCEINLINE FVector2D FVector2D::ClampAxes(float MinAxisVal, float MaxAxisVal) const
{
	return FVector2D(FMath::Clamp(X, MinAxisVal, MaxAxisVal), FMath::Clamp(Y, MinAxisVal, MaxAxisVal));
}


FORCEINLINE FVector2D FVector2D::GetSignVector() const
{
	return FVector2D
		(
		FMath::FloatSelect(X, 1.f, -1.f),
		FMath::FloatSelect(Y, 1.f, -1.f)
		);
}

#pragma endregion;
#endif