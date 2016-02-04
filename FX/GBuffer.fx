
#include "GBufferUtil.fx"



struct Mtrl
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

uniform extern float4x4 gWorldInvTranspose;
uniform extern float4x4 gWorldViewProj;
uniform extern float gFarClipDist;
uniform extern Mtrl      gMtrl;

struct VertexIn
{
	float3 PosL		: POSITION0;
	float3 NormalL	: NORMAL0;
	float2 Tex		: TEXCOORD0;
};


struct VertexOut
{
	float4 PosH			: POSITION;
	float3 NormalW		: NORMAL;
	float2 Tex			: TEXCOORD0;
	float z 			: TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.z = vout.PosH.w;
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = vin.Tex;

	return vout;
}

void PS(VertexOut pin
	,out float4 c0 : COLOR0
	,out float4 c1 : COLOR1
	,out float4 c2 : COLOR2
	//,out float z:SV_Depth
	)
{
	pin.NormalW = normalize(pin.NormalW);
	c0 = StoreGBufferRT0(pin.NormalW, gMtrl.specPower);
	c1 = StoreGBufferRT1(gMtrl.diffuse.xyz, gMtrl.spec.xyz);
	c2 = float4(pin.z, 0,0,0);
	//z = c2;
}

technique GBufferTech
{
    pass P0
    {
		CULLMODE		= CCW;
		ZENABLE			= TRUE;
		ZWRITEENABLE	= true;
		ZFUNC			= LESSEQUAL;
		STENCILENABLE	= false;
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_3_0 VS();
        pixelShader  = compile ps_3_0 PS();
    }
}
