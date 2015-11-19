#ifndef _GRAPHICMATH
#define _GRAPHICMATH
#include "MathUtil.h"
#include "Vector.h"
#include "Box.h"
namespace FMath{

	inline bool LineBoxIntersection
		(
		const FBox&		Box,
		const FVector&	Start,
		const FVector&	End,
		const FVector&	Direction,
		const FVector&	OneOverDirection
		)
	{
		FVector	Time;
		bool	bStartIsOutside = false;

		if (Start.X < Box.Min.X)
		{
			bStartIsOutside = true;
			if (End.X >= Box.Min.X)
			{
				Time.X = (Box.Min.X - Start.X) * OneOverDirection.X;
			}
			else
			{
				return false;
			}
		}
		else if (Start.X > Box.Max.X)
		{
			bStartIsOutside = true;
			if (End.X <= Box.Max.X)
			{
				Time.X = (Box.Max.X - Start.X) * OneOverDirection.X;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Time.X = 0.0f;
		}

		if (Start.Y < Box.Min.Y)
		{
			bStartIsOutside = true;
			if (End.Y >= Box.Min.Y)
			{
				Time.Y = (Box.Min.Y - Start.Y) * OneOverDirection.Y;
			}
			else
			{
				return false;
			}
		}
		else if (Start.Y > Box.Max.Y)
		{
			bStartIsOutside = true;
			if (End.Y <= Box.Max.Y)
			{
				Time.Y = (Box.Max.Y - Start.Y) * OneOverDirection.Y;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Time.Y = 0.0f;
		}

		if (Start.Z < Box.Min.Z)
		{
			bStartIsOutside = true;
			if (End.Z >= Box.Min.Z)
			{
				Time.Z = (Box.Min.Z - Start.Z) * OneOverDirection.Z;
			}
			else
			{
				return false;
			}
		}
		else if (Start.Z > Box.Max.Z)
		{
			bStartIsOutside = true;
			if (End.Z <= Box.Max.Z)
			{
				Time.Z = (Box.Max.Z - Start.Z) * OneOverDirection.Z;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Time.Z = 0.0f;
		}

		if (bStartIsOutside)
		{
			const float	MaxTime = Max3(Time.X, Time.Y, Time.Z);

			if (MaxTime >= 0.0f && MaxTime <= 1.0f)
			{
				const FVector Hit = Start + Direction * MaxTime;
				const float BOX_SIDE_THRESHOLD = 0.1f;
				if (Hit.X > Box.Min.X - BOX_SIDE_THRESHOLD && Hit.X < Box.Max.X + BOX_SIDE_THRESHOLD &&
					Hit.Y > Box.Min.Y - BOX_SIDE_THRESHOLD && Hit.Y < Box.Max.Y + BOX_SIDE_THRESHOLD &&
					Hit.Z > Box.Min.Z - BOX_SIDE_THRESHOLD && Hit.Z < Box.Max.Z + BOX_SIDE_THRESHOLD)
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}

inline bool LineBoxIntersection
	(
	const FBox& Box,
	const FVector& Start,
	const FVector& End,
	const FVector& Direction
	)
{
		return LineBoxIntersection(Box, Start, End, Direction, Direction.Reciprocal());
}



}


#endif // !_GRAPHICMATH
