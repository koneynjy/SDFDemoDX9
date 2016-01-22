


// Total number of direct samples to take at each pixel
#define NUM_SAMPLES (11)

// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower 
// miplevel to maintain reasonable spatial locality in the cache
// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
#define LOG_MAX_OFFSET 3

// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
#define MAX_MIP_LEVEL 5

/** Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling. 
    This need not match the real far plane*/
#define FAR_PLANE_Z (300.0)

// This is the number of turns around the circle that the spiral pattern makes.  This should be prime to prevent
// taps from lining up.  This particular choice was tuned for NUM_SAMPLES == 9
#define NUM_SPIRAL_TURNS (7)

//////////////////////////////////////////////////

/** The height in pixels of a 1m object if viewed from 1m away.  
    You can compute it from your projection matrix.  The actual value is just
    a scale factor on radius; you can simply hardcode this to a constant (~500)
    and make your radius value unitless (...but resolution dependent.)  */
float projScale;

/** Negative, "linear" values in world-space units */
Texture2D<float> CS_Z_buffer;

/** World-space AO radius in scene units (r).  e.g., 1.0m */
float radius;

/** Bias to avoid AO in smooth corners, e.g., 0.01m */
float bias;

/** Darkending factor, e.g., 1.0 */
float intensity;

/**  vec4(-2.0f / (width*P[0][0]), 
          -2.0f / (height*P[1][1]),
          ( 1.0f - P[0][2]) / P[0][0], 
          ( 1.0f + P[1][2]) / P[1][1])
    
    where P is the projection matrix that maps camera space points 
    to [-1, 1] x [-1, 1].  That is, GCamera::getProjectUnit(). */
float4 projInfo;

#define visibility      fragment.color.r
#define bilateralKey    fragment.color.gb

struct PixelInput
{
	float4 position : POSITION;
	float2 texCoords : TEXCOORD0;
};


PixelInput VS(float3 pos : POSITION)
{
	PixelInput vout;
	vout.position = float4(pos.xy, 1.0f, 1.0f);
	vout.texCoords = pos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	return vout;
}

struct PixelOutput
{
	float4 color : COLOR;
};

float radius2 = radius * radius;

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
	return normalize(cross(ddy(C), ddx(C)));
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
	return clamp(z * (1.0 / FAR_PLANE_Z), 0.0, 1.0);
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
float3 getPosition(int2 ssP) {
	float3 P;

	P.z = CS_Z_buffer.Load(int3(ssP, 0)).r;

	// Offset to pixel center
	P = reconstructCSPosition(float2(ssP) + float2(0.5, 0.5), P.z);
	return P;
}


/** Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.  Assumes length(unitOffset) == 1 */
float3 getOffsetPosition(int2 ssC, float2 unitOffset, float ssR) {
	// Derivation:
	//  mipLevel = floor(log(ssR / MAX_OFFSET));
	int mipLevel = clamp((int)floor(log2(ssR)) - LOG_MAX_OFFSET, 0, MAX_MIP_LEVEL);

	int2 ssP = int2(ssR*unitOffset) + ssC;

	float3 P;

	// Divide coordinate by 2^mipLevel
	P.z = CS_Z_buffer.Load(int3(ssP >> mipLevel, mipLevel)).r;

	// Offset to pixel center
	P = reconstructCSPosition(float2(ssP) + float2(0.5, 0.5), P.z);

	return P;
}


/** Compute the occlusion due to sample with index \a i about the pixel at \a ssC that corresponds
    to camera-space point \a C with unit normal \a n_C, using maximum screen-space sampling radius \a ssDiskRadius */
float sampleAO(in int2 ssC, in float3 C, in float3 n_C, in float ssDiskRadius, in int tapIndex, in float randomPatternRotationAngle) {
	// Offset on the unit disk, spun for this pixel
	float ssR;
	float2 unitOffset = tapLocation(tapIndex, randomPatternRotationAngle, ssR);
	ssR *= ssDiskRadius;

	// The occluding point in camera space
	float3 Q = getOffsetPosition(ssC, unitOffset, ssR);

	float3 v = Q - C;

	float vv = dot(v, v);
	float vn = dot(v, n_C);

    const float epsilon = 0.01;
    float f = max(radius2 - vv, 0.0); return f * f * f * max((vn - bias) / (epsilon + vv), 0.0);
}


PixelOutput ps_main(PixelInput pixel, in float4 cpos: VPOS)
{
	PixelOutput fragment;
	fragment.color = float4(0,0,0,0);

	// Pixel being shaded 
	int2 ssC = cpos.xy//��ȡ��Ļ���ꣿ��

	// World space point being shaded
	float3 C = getPosition(ssC);//��ȡ����ռ�����

	packKey(CSZToKey(C.z), bilateralKey);

	// Hash function used in the HPG12 AlchemyAO paper
	float randomPatternRotationAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;

	// Reconstruct normals from positions. These will lead to 1-pixel black lines
	// at depth discontinuities, however the blur will wipe those out so they are not visible
	// in the final image.
	float3 n_C = reconstructCSFaceNormal(C);

	// Choose the screen-space sample radius
	// proportional to the projected area of the sphere
	float ssDiskRadius = -projScale * radius / C.z;

	float sum = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
	     sum += sampleAO(ssC, C, n_C, ssDiskRadius, i, randomPatternRotationAngle);
	}

    float temp = radius2 * radius;
    sum /= temp * temp;
	float A = max(0.0, 1.0 - sum * intensity * (5.0 / NUM_SAMPLES));

	// Bilateral box-filter over a quad for free, respecting depth edges
	// (the difference that this makes is subtle)
	if (abs(ddx(C.z)) < 0.02) {
		A -= ddx(A) * ((ssC.x & 1) - 0.5);
	}
	if (abs(ddy(C.z)) < 0.02) {
		A -= ddy(A) * ((ssC.y & 1) - 0.5);
	}

	visibility = A;

	return fragment;
}
