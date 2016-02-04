

#include<GBufferUtil.fx>
// Total number of direct samples to take at each pixel
#define FALLOFFFUNC 4
#define FALLOFF(i) falloff##i##(v,n_C)

#define HASHFUNC 2
#define HASH(i, pos) HashFunc##i##(pos);

//#define HIZDISABLE
#define NUM_SAMPLES (10)

// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower 
// miplevel to maintain reasonable spatial locality in the cache
// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
#define LOG_MAX_OFFSET 3

// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
#define MAX_MIP_LEVEL 5

/** Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling. 
    This need not match the real far plane*/
//#define FAR_PLANE_Z (300.0)

// This is the number of turns around the circle that the spiral pattern makes.  This should be prime to prevent
// taps from lining up.  This particular choice was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS (7)

//////////////////////////////////////////////////

Texture2D gGBuffer0;
sampler TexGB0 = sampler_state
{
	Texture = <gGBuffer0>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};


/** Negative, "linear" values in world-space units */
Texture2D gCSZBuffer;
sampler TexDepth = sampler_state
{
	Texture = <gCSZBuffer>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexDepthP = sampler_state
{
	Texture = <gCSZBuffer>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = MIRROR; 
    AddressV  = MIRROR;
};

cbuffer cb
{
	/** World-space AO radius in scene units (r).  e.g., 1.0m */
	float gRadius;
	float gRadius2;//r*r
/** Bias to avoid AO in smooth corners, e.g., 0.01m */
	float gBias;

/** Darkending factor, e.g., 1.0 */
	float gIntensity;
	float gIntensityDivR6;

/**  vec4(-2.0f / (width*P[0][0]), 
          -2.0f / (height*P[1][1]),
          ( 1.0f - P[0][2]) / P[0][0], 
          ( 1.0f + P[1][2]) / P[1][1])
    
    where P is the projection matrix that maps camera space points 
    to [-1, 1] x [-1, 1].  That is, GCamera::getProjectUnit(). */
	float4 projInfo;

/** The height in pixels of a 1m object if viewed from 1m away.  
    You can compute it from your projection matrix.  The actual value is just
    a scale factor on radius; you can simply hardcode this to a constant (~500)
    and make your radius value unitless (...but resolution dependent.)  */
	float gProjScale;
	float2 gRes;
	float3 gFarPlane;
	float4x4 gView;
}



#define visibility      fragment.color.r
#define bilateralKey    fragment.color.gb

struct PixelInput
{
	float4 position		: POSITION;
	float2 texCoords	: TEXCOORD0;
};


PixelInput VS(float3 pos : POSITION, float2 tex : TEXCOORD)
{
	PixelInput vout;
	vout.position = float4(pos.xy, 1.0f, 1.0f);
	vout.texCoords = tex;
	return vout;
}

struct PixelOutput
{
	float4 color : COLOR;
};



/////////////////////////////////////////////////////////

/** Reconstruct camera-space P.xyz from screen-space S = (x, y) in
    pixels and camera-space z < 0.  Assumes that the upper-left pixel center
    is at (0.5, 0.5) [but that need not be the location at which the sample tap 
    was placed!]

    Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
  */
float3 reconstructCSPosition(float2 S, float z) {
	return float3((S.xy * projInfo.xy + projInfo.zw) * z, z);
}

/** Reconstructs screen-space unit normal from screen-space position */
float3 reconstructCSFaceNormal(float3 C) {
	return normalize(cross(ddx(C), ddy(C)));
}

/** Returns a unit vector and a screen-space radius for the tap on a unit disk (the caller should scale by the actual disk radius) */
float2 tapLocation(int sampleNumber, float spinAngle, out float ssR){
	// Radius relative to ssR
	float alpha = float(sampleNumber + 0.5) * (1.0 / NUM_SAMPLES);
	float angle = alpha * (NUM_SPIRAL_TURNS * 6.28) + spinAngle;

	ssR = alpha;
	return float2(cos(angle), sin(angle));
}


/** Used for packing Z into the GB channels */
float CSZToKey(float z) {
	return clamp(z / gFarPlane.z, 0.0, 1.0);
}

/** Used for packing Z into the GB channels */
void packKey(float key, out float2 p) {
	// Round to the nearest 1/256.0
	float temp = floor(key * 256.0);

	// Integer part
	p.x = temp * (1.0 / 256.0);

	// Fractional part
	p.y = key * 256.0 - temp;
}

 
/** Read the camera-space position of the point at screen-space pixel ssP */
float3 getPosition(float z, float2 uv) {
	float3 P;

	P.z = z;
	P.xy = (uv - float2(0.5f,0.5f)) * z * gFarPlane.xy;
	// Offset to pixel center
	//P = float3(gViewDirC.xy * P.z / gFarPlane.xy, P.z);
	return P;
}

float3 getPosition(float2 uv, float3 gViewDirC) {
	float3 P;

	P.z = tex2Dlod(TexDepth, float4(uv, 0, 0)).r;
	P = P.z * gViewDirC;
	return P;
}




/** Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.  Assumes length(unitOffset) == 1 */
float3 getOffsetPosition(float2 ssC, float2 unitOffset, float ssR) {
	// Derivation:
	//  mipLevel = floor(log(ssR / MAX_OFFSET));
	int mipLevel = clamp((int)floor(log2(ssR)) - LOG_MAX_OFFSET, 0, MAX_MIP_LEVEL);

	float2 ssP = ssR*unitOffset + ssC;

	float3 P;

	// Divide coordinate by 2^mipLevel
	float2 uv = (ssP) / gRes;
#ifdef HIZDISABLE
	P.z = tex2Dlod(TexDepth, float4(uv , 0, 0)).r;
#else
	P.z = tex2Dlod(TexDepth, float4(uv , 0, mipLevel)).r;
#endif
	// Offset to pixel center
	P = getPosition(P.z, uv);

	return P;
}

float falloff0(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float vn = dot(v, n_C);
	float f = max(1.0 - vv * (1.0 / gRadius2), 0.0); 
	const float epsilon = 0.01;
	return f * max((vn - gBias) * rsqrt(epsilon + vv), 0.0);
}

float falloff1(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float sqvv = sqrt(vv);
	float3 nv = v / sqvv;
	return max(0.0, 1.0 - sqvv * rcp(gRadius)) * max(0.0, dot(nv, n_C) - gBias);
}

float falloff2(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float vn = dot(v, n_C);
	const float epsilon = 0.01;
	float f = max(gRadius2 - vv, 0.0); 
	return f * f * f * max((vn - gBias) * rcp(epsilon + vv), 0.0);
}

float falloff3(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float vn = dot(v, n_C);
	const float epsilon = 0.01;
	return float(vv < gRadius2) * max((vn - gBias) * rcp(epsilon + vv), 0.0) * gRadius2 * 0.6;
}

float falloff4(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float vn = dot(v, n_C);
	const float epsilon = 0.01;
	return 4.0 * max(1.0 - vv * 1.0 / gRadius2, 0.0) * max(vn - gBias, 0.0);
}


float falloff5(float3 v, float3 n_C)
{
	float vv = dot(v, v);
	float vn = dot(v, n_C);
	const float epsilon = 0.01;
	return 2.0 * float(vv < gRadius2) * max(vn - gBias, 0.0);
}



/** Compute the occlusion due to sample with index \a i about the pixel at \a ssC that corresponds
    to camera-space point \a C with unit normal \a n_C, using maximum screen-space sampling radius \a ssDiskRadius */
float sampleAO(in int2 ssC, in float3 C, in float3 n_C, 
			   in float ssDiskRadius, in int tapIndex, in float randomPatternRotationAngle) {
	// Offset on the unit disk, spun for this pixel
	float ssR;
	float2 unitOffset = tapLocation(tapIndex, randomPatternRotationAngle, ssR);
	ssR *= ssDiskRadius;

	// The occluding point in camera space
	float3 Q = getOffsetPosition(ssC, unitOffset, ssR);
	float3 v = Q - C;
	
	return FALLOFF(FALLOFFFUNC);
}

// by Morgan McGuire https://www.shadertoy.com/view/4dS3Wd
// All noise functions are designed for values on integer scale.
// They are tuned to avoid visible periodicity for both positive and
// negative coordinates within a few orders of magnitude.
float hash(float n) { return frac(sin(n) * 1e4); }
float hash(float2 p) { return frac(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise1(float2 x) {
    float2 i = floor(x);
    float2 f = frac(x);

	// Four corners in 2D of a tile
	float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
	// return float3(lerp(lerp(a, b, smoothstep(0.0, 1.0, f.x)),
	//			lerp(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
    float2 u = f * f * (3.0 - 2.0 * f);
	return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

int XOR(int x, int y)
{
	int res = 0;
	int v = 1;
	for (int i = 0; i < 11;i ++)
	{
		res += (x % 2 + y % 2) % 2 * v;
		x /= 2;
		y /= 2;
		v *= 2;
	}
	return res;
}

float HashFunc0(float2 pos)
{
	return (3 * XOR(pos.x , pos.y) + pos.x * pos.y) * 10;
}

float HashFunc1(float2 pos)
{
	return (3 * (pos.x + pos.y) + pos.x * pos.y) * 10;
}

float HashFunc2(float2 pos)
{
	return noise1(pos);
}

///////////////////////////////////////////////////////////

PixelOutput PS(PixelInput pixel, in float4 cpos:VPOS)
{
	PixelOutput fragment;
	fragment.color = float4(0,0,0,0);

	// Pixel being shaded 
	float2 ssC = pixel.texCoords * gRes;//cpos.xy + float2(0.5, 0.5);//»ñÈ¡ÆÁÄ»×ø±ê£¿£¿
	//float2 uv = (ssC) / gRes;
	//float z =  tex2Dlod(TexDepth, float4(uv, 0, 0)).r;
	float z =  tex2Dlod(TexDepth, float4(pixel.texCoords, 0, 0)).r;
	
	// World space point being shaded
	float3 C = getPosition(z, pixel.texCoords);

	packKey(z / gFarPlane.z, bilateralKey);
	//fragment.color.rgb = float4(z / gFarPlane.z, 0 , 0 , 0);
	//return fragment;
	// Hash function used in the HPG12 AlchemyAO paper
	float randomPatternRotationAngle = HASH(HASHFUNC, ssC);//noise1(ssC)*10;//

	// Reconstruct normals from positions. These will lead to 1-pixel black lines
	// at depth discontinuities, however the blur will wipe those out so they are not visible
	// in the final image.
	float3 n_C = reconstructCSFaceNormal(C);

	//float3 n_C = mul(GetNormal(tex2D(TexGB0, pixel.texCoords)), (float3x3)gView);
	//fragment.color.rgb = n_C;
	//return fragment;
	
	// Choose the screen-space sample radius
	// proportional to the projected area of the sphere
	float ssDiskRadius = -gProjScale * gRadius / C.z;

	float sum = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
	     sum += sampleAO(ssC, C, n_C, ssDiskRadius, i, randomPatternRotationAngle);
	}
	//fragment.color.rgb = float3(sum, 0,0);
	//return fragment;
	//float temp = gRadius2 * gRadius;
	//sum /= temp * temp;
#if FALLOFFFUNC  == 0
	float A = pow(max(0.0, 1.0 - sqrt(sum * (3.0 / NUM_SAMPLES))), gIntensity);
#elif FALLOFFFUNC == 2
	float A = max(0.0, 1.0 - sum * gIntensityDivR6 * (5.0 / NUM_SAMPLES));
#else
	float A = max(0.0, 1.0 - sum * gIntensity * (1.0 / NUM_SAMPLES));
#endif
	// Bilateral box-filter over a quad for free, respecting depth edges
	// (the difference that this makes is subtle)
	//if (abs(ddx(C.z)) < 0.02) {
	//	A -= ddx(A) * (cpos.x % 2 - 0.5);
	//}
	//if (abs(ddy(C.z)) < 0.02) {
	//	A -= ddy(A) * (cpos.y % 2 - 0.5);
	//}

	visibility = A;

	return fragment;
}


technique SAOTech
{
	pass P0
	{
		CULLMODE		= CCW;
		ZENABLE			= FALSE;
		STENCILENABLE	= false;
		vertexShader = compile vs_3_0 VS();
        pixelShader  = compile ps_3_0 PS();
	}
};