/**
* Implements a thread-safe SRand based RNG.
*
* Very bad quality in the lower bits. Don't use the modulus (%) operator.
*/
#ifndef _RANDOMSTREAM
#define _RANDOMSTREAM

#include "Config.h"
#include "MathUtil.h"
#include "Vector.h"

struct FRandomStream
{
public:

	/**
	* Default constructor.
	*
	* The seed should be set prior to use.
	*/
	FRandomStream()
		: InitialSeed(0)
		, Seed(0)
	{ }

	/**
	* Creates and initializes a new random stream from the specified seed value.
	*
	* @param InSeed The seed value.
	*/
	FRandomStream(int32 InSeed)
		: InitialSeed(InSeed)
		, Seed(InSeed)
	{ }

public:

	/**
	* Initializes this random stream with the specified seed value.
	*
	* @param InSeed The seed value.
	*/
	void Initialize(int32 InSeed)
	{
		InitialSeed = InSeed;
		Seed = InSeed;
	}

	/**
	* Resets this random stream to the initial seed value.
	*/
	void Reset() const
	{
		Seed = InitialSeed;
	}

	int32 GetInitialSeed() const
	{
		return InitialSeed;
	}

	/**
	* Generates a new random seed.
	*/
	void GenerateNewSeed()
	{
		Initialize(FMath::Rand());
	}

	/**
	* Returns a random number between 0 and 1.
	*
	* @return Random number.
	*/
	float GetFraction() const
	{
		MutateSeed();

		const float SRandTemp = 1.0f;
		float Result;

		*(int32*)&Result = (*(int32*)&SRandTemp & 0xff800000) | (Seed & 0x007fffff);

		return FMath::Fractional(Result);
	}

	/**
	* Returns a random number between 0 and MAXUINT.
	*
	* @return Random number.
	*/
	uint32 GetUnsignedInt() const
	{
		MutateSeed();

		return *(uint32*)&Seed;
	}

	/**
	* Returns a random vector of unit size.
	*
	* @return Random unit vector.
	*/
	FVector GetUnitVector() const
	{
		FVector Result;
		float L;

		do
		{
			// Check random vectors in the unit sphere so result is statistically uniform.
			Result.X = GetFraction() * 2.f - 1.f;
			Result.Y = GetFraction() * 2.f - 1.f;
			Result.Z = GetFraction() * 2.f - 1.f;
			L = Result.SizeSquared();
		} while (L > 1.f || L < KINDA_SMALL_NUMBER);

		return Result.GetUnsafeNormal();
	}

	/**
	* Gets the current seed.
	*
	* @return Current seed.
	*/
	int32 GetCurrentSeed() const
	{
		return Seed;
	}

	/**
	* Mirrors the random number API in FMath
	*
	* @return Random number.
	*/
	FORCEINLINE float FRand() const
	{
		return GetFraction();
	}

	/**
	* Helper function for rand implementations.
	*
	* @return A random number in [0..A)
	*/
	FORCEINLINE int32 RandHelper(int32 A) const
	{
		// Can't just multiply GetFraction by A, as GetFraction could be == 1.0f
		return ((A > 0) ? FMath::TruncToInt(GetFraction() * ((float)A - DELTA)) : 0);
	}

	/**
	* Helper function for rand implementations.
	*
	* @return A random number >= Min and <= Max
	*/
	FORCEINLINE int32 RandRange(int32 Min, int32 Max) const
	{
		const int32 Range = (Max - Min) + 1;

		return Min + RandHelper(Range);
	}

	/**
	* Helper function for rand implementations.
	*
	* @return A random number >= Min and <= Max
	*/
	FORCEINLINE float FRandRange(float InMin, float InMax) const
	{
		return InMin + (InMax - InMin) * FRand();
	}

	/**
	* Returns a random vector of unit size.
	*
	* @return Random unit vector.
	*/
	FORCEINLINE FVector VRand() const
	{
		return GetUnitVector();
	}

protected:

	/**
	* Mutates the current seed into the next seed.
	*/
	void MutateSeed() const
	{
		Seed = (Seed * 196314165) + 907633515;
	}

private:

	// Holds the initial seed.
	int32 InitialSeed;

	// Holds the current seed.
	mutable int32 Seed;
};

#endif // !_RANDOMSTREAM
