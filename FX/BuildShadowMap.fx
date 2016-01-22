//=============================================================================
// ShadowMap.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Shadow map demo.
//=============================================================================

struct Mtrl
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct SpotLight
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 posW;
	float3 dirW;  
	float  spotPower;
};

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInvTrans;
uniform extern float4x4 gProjTexMtx;
uniform extern float4x4 gWVP;
uniform extern Mtrl     gMtrl;
uniform extern SpotLight gLight;
uniform extern float3   gEyePosW;
uniform extern float3   gLightPosL;
 
uniform extern float4x4 gLightWVP;
uniform extern float3   gLightPosL;

void BuildShadowMapVS(float3 posL : POSITION0,
                      out float4 posH : POSITION0,
                      out float  depth : TEXCOORD0)
{
	// Render from light's perspective.
	posH = mul(float4(posL, 1.0f), gLightWVP);
	
	// Depth from vertex to light source.
	depth = distance(posL, gLightPosL);
}

void BuildShadowMapPS(float depth : TEXCOORD0)
{
	// Each pixel in the shadow map stores the pixel depth from the light source.
	return depth; 
}

technique BuildShadowMap
{
	pass P0
	{
		vertexShader = compile vs_3_0 BuildShadowMapVS();
        pixelShader  = compile ps_3_0 BuildShadowMapPS();
	}
}