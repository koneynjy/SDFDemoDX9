cbuffer cbPerFrame
{
	float3 gInfo;//x:width y:hight z:miplevel
};

Texture2D gDepthMap;
sampler TexDepth = sampler_state
{
	Texture = <gDepthMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

struct VertexIn
{
	float3 PosH		: POSITION;
	float2 tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH			: POSITION;
	float2 texCoords	: TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosH.xy, 1.0f, 1.0f);
	vout.texCoords = vin.tex;
	return vout;
}

int pack(int x)
{
	return !(x % 2);
}

float4 PS(VertexOut pin, in float4 cpos : VPOS) : COLOR0
{
	int2 ssP = cpos.xy + float2(0.5,0.5);
	int2 nssp = ssP * 2 + int2(pack(ssP.y), pack(ssP.x));
	//return float4(0.5,0,0,0);
	return float4(tex2Dlod(TexDepth, float4((nssp+float2(0.5,0.5)) / gInfo.xy, 0, gInfo.z)).r, 0, 0, 0);
}

technique HIZTech
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


