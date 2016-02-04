//=============================================================================
// DebugTexture.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Effect used to view a texture for debugging.
//=============================================================================
 
float4x4  gWorldViewProj;
Texture2D gTexture;
//Texture3D gSDF;

cbuffer cbPerFrame
{
	float d;
	float2 gRes;
};

cbuffer cbPerSDF
{
	float maxvalue;
};

sampler TexDepth = sampler_state
{
	Texture = <gTexture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexDepthP = sampler_state
{
	Texture = <gTexture>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};
struct VertexIn
{
	float3 PosL		: POSITION;
	float2 tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH : POSITION;
	float2 Tex  : TEXCOORD;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosH = float4(vin.PosL.xy,1.0f, 1.0f);
	vout.Tex = vin.tex;
	return vout;
}

float4 PS(VertexOut pin, in float4 cpos:VPOS) : COLOR0
{ 
	//uint s = gIntTexture[pin.Tex * int2(800,600)].y;
	//float sf = s / 255.0f ;
	//return sf.rrrr;
	
	float4 ao = tex2D(TexDepth,(cpos.xy + float2(0.50,0.50)) / gRes);
	return ao.rrrr;//((clamp(1.0 - (1.0 - ao) * 0.5, 0.0, 1.0) + 0.1) /  (1.0 + 0.1)).rrrr;
	//float v = gSDF.Sample(samLinear, float3(pin.Tex.x, 1.0f - pin.Tex.y, d)).r;
	//if (v > 0)
	//{
	//	v *= 0.5f;
	//}
	//else
	//{
	//	v *= -0.5f;
	//	v = 1.0f - v;
	//}

	//return v.rrrr;
}
 
technique ViewArgbTech
{
    pass P0
    {
		CULLMODE		= CCW;
		ZENABLE			= FALSE;
		STENCILENABLE	= false;
        vertexShader = compile vs_3_0 VS();
        pixelShader  = compile ps_3_0 PS();
    }
}