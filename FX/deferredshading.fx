#include "GBufferUtil.fx"
struct SpotLight
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 posW;
	float3 dirW;  
	float  spotPower;
};

cbuffer cbPerFrame
{
	SpotLight gDirLights;
	float4x4 gViewInv;
	float3 gEyePosW;
	float4 amb = float4(0.3,0.3,0.3,0);
};

void ComputeDirectionalLight(float4 amb, float4 diff, float spec, float gloss, SpotLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dirW;

		// Add ambient term.
	ambient = amb ;//* L.ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = 0;//= pow(max(dot(v, normalize(toEye)), 0.0f), 30.0f);

		diffuse = diffuseFactor * diff * L.diffuse;
		specular = spec * specFactor * L.spec;
	}
}




Texture2D gDepthMap;
Texture2D gGBuffer0;
Texture2D gGBuffer1;
Texture2D gSDFShadow;

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

sampler TexGBuffer1 = sampler_state
{
	Texture = <gGBuffer1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexSDFShadow = sampler_state
{
	Texture = <gSDFShadow>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};



struct VertexIn
{
	float3 PosH    : POSITION;
};

struct VertexOut
{
	float4 PosH			: POSITION;
	float3 viewDirW		: NORMAL;
	float2 uv			: TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.viewDirW = mul(vin.PosH, (float3x3)gViewInv) / vin.PosH.z;//transform view space point to World vector
	vin.PosH.xy /= abs(vin.PosH.xy);//to -1 ~ 1
	vout.PosH = float4(vin.PosH.xy, 1.0f, 1.0f);
	vout.uv = vin.PosH.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);//screen cord uv

	return vout;
}

float4 PS(VertexOut pin) : COLOR0
{
	float depth = tex2D(TexDepth, pin.uv).r;
	float3 v = pin.viewDirW * depth;
	float3 posW = v + gEyePosW;//ReBuild World Position
	float4 gb0 = tex2D(TexGBuffer0, pin.uv);
	float4 gb1 = tex2D(TexGBuffer1, pin.uv);
	float3 normalW = GetNormal(gb0);
	float gloss = GetShininess(gb0);
	float4 diff = float4(gb1.xyz, 0.0f);
	float spec = gb1.w ;
	float3 toEye = -v;
	//
	// Lighting.
	//

	float4 litColor;
	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Only the first light casts a shadow.
	float shadow = 1.0f; //= tex2D(TexSDFShadow, pin.uv).r;
	float4 A, D, S;
	ComputeDirectionalLight(amb, diff, spec, gloss, gDirLights, normalW, toEye,
			A, D, S);

	ambient += A;
	diffuse += shadow * D;
	specular += shadow * S;

	litColor = ambient + diffuse + specular;

	return litColor;
}

technique DeferredShadingTech
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