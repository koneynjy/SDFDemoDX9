
#include"GBufferUtil.fx"
texture3D gSDF0;
texture2D gDepthMap;
texture2D gGBuffer0;

cbuffer cbPerFrame
{
	float4x4 gViewProj;
	float3 gLightDir;
	float3 gEyePosW;
	float  gFarClipDist;
};

cbuffer cbPerModel
{
	float4x4 gSDFToWordInv0;
	float3 gSDFBounds0;
	float gSDFRes0;
};

sampler TexDepth = sampler_state
{
	Texture = <gDepthMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexGBuffer0 = sampler_state
{
	Texture = <gGBuffer0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexSDF = sampler_state
{
	Texture = <gSDF0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
	AddressW  = CLAMP;
};

struct VertexIn
{
	float3 PosH		: POSITION;
};

struct VertexOut
{
	float4 PosH			: POSITION;
	float3 viewDirW		: POSITION1;
	float2 uv			: TEXCOORD0;
	float z				: TEXCOORD1;
};

void CullVS(float3 posH: POSITION
	, out float4 pos : POSITION)
{
	pos = mul(float4(posH, 1.0f), gViewProj);
}

uniform float3 zero = float3(-1e-4f, -1e-4f, -1e-4f);

bool inBounds(float3 start, float3 bounds)
{
	return all(start >= zero) && all(start <= bounds);
}

#define CULLING

bool IntersectBoundsCull(float3 start, float3 dir, float3 bounds,
	out float3 i1, out float length)
{
	i1 = float3(0, 0, 0);
	length = 0;
	float3 tMin = -start / dir;
	float3 tMax = (bounds - start) / dir;
	bounds += float3(1e-4f, 1e-4f, 1e-4f);
	float mint = 3.40282e+038f, maxt = 0;
	[unroll]
	for (int i = 0; i < 3; i++)
	{
		if (inBounds(start + tMin[i] * dir, bounds))
		{
			mint = min(tMin[i], mint);
			maxt = max(tMin[i], maxt);
		}

		if (inBounds(start + tMax[i] * dir, bounds))
		{
			mint = min(tMax[i], mint);
			maxt = max(tMax[i], maxt);
		}
	}

	mint = max(0, mint);
	i1 = start + dir * mint;
	length = maxt - mint;
	return true;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.viewDirW = vin.PosH - gEyePosW;
	vout.PosH = mul(float4(vin.PosH, 1.0f), gViewProj);
	vout.uv = vout.PosH.xy;
	vout.z = vout.PosH.w;
	return vout;
}

#define TOTALSTEP 32

float4 PS(VertexOut pin) : COLOR
{
	//return 0.5.rrrr;
	float2 uv = pin.uv / pin.z;
	uv = uv * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	float depth = tex2D(TexDepth, uv).r;

	float3 v = pin.viewDirW * (depth * gFarClipDist / pin.z);
		//return v.zzzz / 50;
	float3 posW = v + gEyePosW;//ReBuild World Position
	float3 posSDF = mul(float4(posW, 1.0), gSDFToWordInv0).xyz;
	float4 gb0 = tex2D(TexGBuffer0, uv);
	float3 normalW = GetNormal(gb0);
	float3 dir = -gLightDir;
	clip(dot(normalW, dir));
	float3 i1 = float3(0,0,0), i2;
	float length = 0, len = 0;
	float4 shadow = float4(1.0f, 0, 0, 0);
	float boundMax = gSDFRes0;
	float minstep;
#ifdef CULLING
	IntersectBoundsCull(posSDF, dir, gSDFBounds0, i1, length);
#else
	if (IntersectBoundsCull(posSDF, dir, gSDFBounds0, i1, length))
	{
#endif
		//return float4(i1/ gSDFBounds0, 1.0f);
		float3 start = i1;
		float3 scale = float3(1.0f,1.0f,1.0f) / gSDFBounds0;
		float step = tex3D(TexSDF, start * scale).r * boundMax;
		float smallstep = length / TOTALSTEP;
		float ori = 0.01f * boundMax;
		start += ori * dir;
		len += ori;
		minstep = ori * TOTALSTEP / len;
		[unroll(TOTALSTEP)]
		for (int i = 0; i <= TOTALSTEP; ++i)
		{
			if (len >= length) break;
			//float3 uvw = start * scale;
			step = tex3D(TexSDF, start * scale).r;
			if (step <= 0){
				shadow.r = 0.0f;
				break;
			}
			step *= boundMax;
			minstep = min(minstep, step * TOTALSTEP / len);
			
			if (step < smallstep) step = smallstep;
			//step *= 1.25f;
			len += step;
			start += step * dir;
		}
#ifndef	CULLING
	}
#endif
	
	if (minstep < 1.0f) shadow *= minstep;	
	return shadow;
}

technique SDFShadowTech
{
	pass P0
	{
		CULLMODE		= CW;
		ZENABLE			= TRUE;
		ZWRITEENABLE	= false;
		ZFUNC			= GREATEREQUAL;
		STENCILENABLE	= true;
		STENCILREF		= 0xff;
		STENCILFAIL		= KEEP;
		STENCILZFAIL	= KEEP;
		STENCILPASS		= REPLACE;
		STENCILFUNC		= ALWAYS;
		CCW_STENCILFAIL	= KEEP;
		CCW_STENCILZFAIL= KEEP;
		CCW_STENCILPASS	= REPLACE;
		CCW_STENCILFUNC	= ALWAYS;
		vertexShader = compile vs_3_0 CullVS();
        pixelShader  = NULL;
	}
	pass P1
	{
		CULLMODE		= CCW;
		ZENABLE			= TRUE;
		ZWRITEENABLE	= false;
		ZFUNC			= LESSEQUAL;
		STENCILENABLE	= true;
		STENCILREF		= 0xff;
		STENCILFAIL		= KEEP;
		STENCILZFAIL	= ZERO;
		STENCILPASS		= KEEP;
		STENCILFUNC		= EQUAL;
		CCW_STENCILFAIL	= KEEP;
		CCW_STENCILZFAIL= KEEP;
		CCW_STENCILPASS	= REPLACE;
		CCW_STENCILFUNC	= NEVER;
		vertexShader = compile vs_3_0 VS();
        pixelShader  = compile ps_3_0 PS();
	}
};



