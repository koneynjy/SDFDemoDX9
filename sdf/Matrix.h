// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#ifndef _MATRIX
#define _MATRIX
#pragma once;
#include "Config.h"
#include "Vector.h"

/**
* 4x4 matrix of floating point values.
* Matrix-matrix multiplication happens with a pre-multiple of the transpose --
* in other words, Res = Mat1.operator*(Mat2) means Res = Mat2^T * Mat1, as
* opposed to Res = Mat1 * Mat2.
* Matrix elements are accessed with M[RowIndex][ColumnIndex].
*/
struct FMatrix
{
public:
	union
	{
		MS_ALIGN(16) float M[4][4];
	};

	//Identity matrix
	MS_ALIGN(16) static const FMatrix Identity;

	// Constructors.
	FORCEINLINE FMatrix();

	/**
	* Constructor.
	*
	* @param EForceInit Force Init Enum.
	*/
	explicit FORCEINLINE FMatrix(EForceInit)
	{
		memset(M, 0, sizeof(M));
	}

	/**
	* Constructor.
	*
	* @param InX X plane
	* @param InY Y plane
	* @param InZ Z plane
	* @param InW W plane
	*/
	FORCEINLINE FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW);

	// Set this to the identity matrix
	inline void SetIdentity();

	/**
	* Gets the result of multiplying a Matrix to this.
	*
	* @param Other The matrix to multiply this by.
	* @return The result of multiplication.
	*/
	FORCEINLINE FMatrix operator* (const FMatrix& Other) const;

	/**
	* Multiply this by a matrix.
	*
	* @param Other the matrix to multiply by this.
	* @return reference to this after multiply.
	*/
	FORCEINLINE void operator*=(const FMatrix& Other);

	/**
	* Gets the result of adding a matrix to this.
	*
	* @param Other The Matrix to add.
	* @return The result of addition.
	*/
	FORCEINLINE FMatrix operator+ (const FMatrix& Other) const;

	/**
	* Adds to this matrix.
	*
	* @param Other The matrix to add to this.
	* @return Reference to this after addition.
	*/
	FORCEINLINE void operator+=(const FMatrix& Other);

	/**
	* This isn't applying SCALE, just multiplying float to all members - i.e. weighting
	*/
	FORCEINLINE FMatrix operator* (float Other) const;

	/**
	* Multiply this matrix by a weighting factor.
	*
	* @param other The weight.
	* @return a reference to this after weighting.
	*/
	FORCEINLINE void operator*=(float Other);

	/**
	* Checks whether two matrix are identical.
	*
	* @param Other The other matrix.
	* @return true if two matrix are identical, otherwise false.
	*/
	inline bool operator==(const FMatrix& Other) const;

	/**
	* Checks whether another Matrix is equal to this, within specified tolerance.
	*
	* @param Other The other Matrix.
	* @param Tolerance Error Tolerance.
	* @return true if two Matrix are equal, within specified tolerance, otherwise false.
	*/
	inline bool Equals(const FMatrix& Other, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether another Matrix is not equal to this, within specified tolerance.
	*
	* @param Other The other Matrix.
	* @return true if two Matrix are not equal, within specified tolerance, otherwise false.
	*/
	inline bool operator!=(const FMatrix& Other) const;

	// Homogeneous transform.
	FORCEINLINE FVector4 TransformFVector4(const FVector4& V) const;

	/** Transform a location - will take into account translation part of the FMatrix. */
	FORCEINLINE FVector4 TransformPosition(const FVector4 &V) const;
													   
	/** Inverts the matrix and then transforms V - correctly handles scaling in this matrix. */
	FORCEINLINE FVector4 InverseTransformPosition(const FVector4 &V) const;

	/**
	*	Transform a direction vector - will not take into account translation part of the FMatrix.
	*	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT.
	*/
	FORCEINLINE FVector4 TransformVector(const FVector& V) const;

	/**
	*	Transform a direction vector by the inverse of this matrix - will not take into account translation part.
	*	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT with adjoint of matrix inverse.
	*/
	FORCEINLINE FVector4 InverseTransformVector(const FVector4 &V) const;

	// Transpose.

	FORCEINLINE FMatrix GetTransposed() const;

	// @return determinant of this matrix.

	inline float Determinant() const;

	/** @return the determinant of rotation 3x3 matrix */
	inline float RotDeterminant() const;

	/** Fast path, doesn't check for nil matrices in final release builds */
	inline FMatrix InverseFast() const;

	/** Fast path, and handles nil matrices. */
	inline FMatrix Inverse() const;

	inline FMatrix TransposeAdjoint() const;

	// NOTE: There is some compiler optimization issues with WIN64 that cause FORCEINLINE to cause a crash
	// Remove any scaling from this matrix (ie magnitude of each row is 1) with error Tolerance
	inline void RemoveScaling(float Tolerance = SMALL_NUMBER);

	// Returns matrix after RemoveScaling with error Tolerance
	inline FMatrix GetMatrixWithoutScale(float Tolerance = SMALL_NUMBER) const;

	/** Remove any scaling from this matrix (ie magnitude of each row is 1) and return the 3D scale vector that was initially present with error Tolerance */
	inline FVector4 ExtractScaling(float Tolerance = SMALL_NUMBER);

	/** return a 3D scale vector calculated from this matrix (where each component is the magnitude of a row vector) with error Tolerance. */
	inline FVector4 GetScaleVector(float Tolerance = SMALL_NUMBER) const;

	// Remove any translation from this matrix
	inline FMatrix RemoveTranslation() const;

	/** Returns a matrix with an additional translation concatenated. */
	inline FMatrix ConcatTranslation(const FVector4& Translation) const;

	/** Returns true if any element of this matrix is NaN */
	inline bool ContainsNaN() const;

	/** Scale the translation part of the matrix by the supplied vector. */
	inline void ScaleTranslation(const FVector4& Scale3D);

	/** @return the maximum magnitude of any row of the matrix. */
	inline float GetMaximumAxisScale() const;

	/** Apply Scale to this matrix **/
	inline FMatrix ApplyScale(float Scale);

	// @return the origin of the co-ordinate system
	inline FVector4 GetOrigin() const;

	/**
	* Convert this Atom to the 3x4 transpose of the transformation matrix.
	*/
	void To3x4MatrixTranspose(float* Out) const
	{
		const float* RESTRICT Src = &(M[0][0]);
		float* RESTRICT Dest = Out;

		Dest[0] = Src[0];   // [0][0]
		Dest[1] = Src[4];   // [1][0]
		Dest[2] = Src[8];   // [2][0]
		Dest[3] = Src[12];  // [3][0]

		Dest[4] = Src[1];   // [0][1]
		Dest[5] = Src[5];   // [1][1]
		Dest[6] = Src[9];   // [2][1]
		Dest[7] = Src[13];  // [3][1]

		Dest[8] = Src[2];   // [0][2]
		Dest[9] = Src[6];   // [1][2]
		Dest[10] = Src[10]; // [2][2]
		Dest[11] = Src[14]; // [3][2]
	}
};

MS_ALIGN(16) const FMatrix FMatrix::Identity = 
FMatrix(
FVector4( 1, 0, 0, 0 ), 
FVector4( 0, 1, 0, 0 ),
FVector4( 0, 0, 1, 0 ),
FVector4( 0, 0, 0, 1 )
);

FORCEINLINE FMatrix::FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW)
{
	M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = InX.W;
	M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = InY.W;
	M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = InZ.W;
	M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = InW.W;
}

FORCEINLINE FVector4 FMatrix::TransformFVector4(const FVector4 &P) const
{
	FVector4 Result;
	VectorRegister VecP = VectorLoadAligned(&P);
	VectorRegister VecR = VectorTransformVector(VecP, this);
	VectorStoreAligned(VecR, &Result);
	return Result;
}

/**
*	Transform a direction vector - will not take into account translation part of the FMatrix.
*	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT.
*/
FORCEINLINE FVector4 FMatrix::TransformVector(const FVector& V) const
{
	return TransformFVector4(FVector4(V.X, V.Y, V.Z, 0.0f));
}


/**
* A storage class for compile-time fixed size matrices.
*/
template<uint32 NumRows, uint32 NumColumns>
class TMatrix
{
public:

	// Variables.
	MS_ALIGN(16) float M[NumRows][NumColumns];

	// Constructor
	TMatrix();

	/**
	* Constructor
	*
	* @param InMatrix FMatrix reference
	*/
	TMatrix(const FMatrix& InMatrix);
};


template<uint32 NumRows, uint32 NumColumns>
FORCEINLINE TMatrix<NumRows, NumColumns>::TMatrix() { }


template<uint32 NumRows, uint32 NumColumns>
FORCEINLINE TMatrix<NumRows, NumColumns>::TMatrix(const FMatrix& InMatrix)
{
	for (uint32 RowIndex = 0; (RowIndex < NumRows) && (RowIndex < 4); RowIndex++)
	{
		for (uint32 ColumnIndex = 0; (ColumnIndex < NumColumns) && (ColumnIndex < 4); ColumnIndex++)
		{
			M[RowIndex][ColumnIndex] = InMatrix.M[RowIndex][ColumnIndex];
		}
	}
}


// very high quality 4x4 matrix inverse
static inline void Inverse4x4(double* dst, const float* src)
{
	const double s0 = (double)(src[0]); const double s1 = (double)(src[1]); const double s2 = (double)(src[2]); const double s3 = (double)(src[3]);
	const double s4 = (double)(src[4]); const double s5 = (double)(src[5]); const double s6 = (double)(src[6]); const double s7 = (double)(src[7]);
	const double s8 = (double)(src[8]); const double s9 = (double)(src[9]); const double s10 = (double)(src[10]); const double s11 = (double)(src[11]);
	const double s12 = (double)(src[12]); const double s13 = (double)(src[13]); const double s14 = (double)(src[14]); const double s15 = (double)(src[15]);

	double inv[16];
	inv[0] = s5 * s10 * s15 - s5 * s11 * s14 - s9 * s6 * s15 + s9 * s7 * s14 + s13 * s6 * s11 - s13 * s7 * s10;
	inv[1] = -s1 * s10 * s15 + s1 * s11 * s14 + s9 * s2 * s15 - s9 * s3 * s14 - s13 * s2 * s11 + s13 * s3 * s10;
	inv[2] = s1 * s6  * s15 - s1 * s7  * s14 - s5 * s2 * s15 + s5 * s3 * s14 + s13 * s2 * s7 - s13 * s3 * s6;
	inv[3] = -s1 * s6  * s11 + s1 * s7  * s10 + s5 * s2 * s11 - s5 * s3 * s10 - s9  * s2 * s7 + s9  * s3 * s6;
	inv[4] = -s4 * s10 * s15 + s4 * s11 * s14 + s8 * s6 * s15 - s8 * s7 * s14 - s12 * s6 * s11 + s12 * s7 * s10;
	inv[5] = s0 * s10 * s15 - s0 * s11 * s14 - s8 * s2 * s15 + s8 * s3 * s14 + s12 * s2 * s11 - s12 * s3 * s10;
	inv[6] = -s0 * s6  * s15 + s0 * s7  * s14 + s4 * s2 * s15 - s4 * s3 * s14 - s12 * s2 * s7 + s12 * s3 * s6;
	inv[7] = s0 * s6  * s11 - s0 * s7  * s10 - s4 * s2 * s11 + s4 * s3 * s10 + s8  * s2 * s7 - s8  * s3 * s6;
	inv[8] = s4 * s9  * s15 - s4 * s11 * s13 - s8 * s5 * s15 + s8 * s7 * s13 + s12 * s5 * s11 - s12 * s7 * s9;
	inv[9] = -s0 * s9  * s15 + s0 * s11 * s13 + s8 * s1 * s15 - s8 * s3 * s13 - s12 * s1 * s11 + s12 * s3 * s9;
	inv[10] = s0 * s5  * s15 - s0 * s7  * s13 - s4 * s1 * s15 + s4 * s3 * s13 + s12 * s1 * s7 - s12 * s3 * s5;
	inv[11] = -s0 * s5  * s11 + s0 * s7  * s9 + s4 * s1 * s11 - s4 * s3 * s9 - s8  * s1 * s7 + s8  * s3 * s5;
	inv[12] = -s4 * s9  * s14 + s4 * s10 * s13 + s8 * s5 * s14 - s8 * s6 * s13 - s12 * s5 * s10 + s12 * s6 * s9;
	inv[13] = s0 * s9  * s14 - s0 * s10 * s13 - s8 * s1 * s14 + s8 * s2 * s13 + s12 * s1 * s10 - s12 * s2 * s9;
	inv[14] = -s0 * s5  * s14 + s0 * s6  * s13 + s4 * s1 * s14 - s4 * s2 * s13 - s12 * s1 * s6 + s12 * s2 * s5;
	inv[15] = s0 * s5  * s10 - s0 * s6  * s9 - s4 * s1 * s10 + s4 * s2 * s9 + s8  * s1 * s6 - s8  * s2 * s5;

	double det = s0 * inv[0] + s1 * inv[4] + s2 * inv[8] + s3 * inv[12];
	if (det != 0.0)
	{
		det = 1.0 / det;
	}
	for (int i = 0; i < 16; i++)
	{
		dst[i] = inv[i] * det;
	}
}

#endif
