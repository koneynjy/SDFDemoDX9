cbuffer cbConstant
{
	float gFarClipDist;
};

cbuffer cbPerFrame
{
	float4x4 gViewProj;
};

struct VertexOut
{
	float4 pos:		POSITION;
};

VertexOut VS(float3 posH: POSITION)
{
	VertexOut vout;
	vout.pos =  mul(float4(posH, 1.0f), gViewProj);
	return vout;
}

void PS(VertexOut pin){}

technique SDFVolumeDepthTech
{
	pass P0
	{
		vertexShader = compile vs_3_0 VS();
		pixelShader = NULL;
	}
};