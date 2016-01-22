
float4 StoreGBufferRT0(float3 normal, float shininess)
{
	float p = sqrt(-normal.z * 8 + 8);
	float2 enc = normal.xy / p + 0.5f;
	float2 enc255 = enc * 255;
	float2 residual = floor(frac(enc255) * 16);
	return float4(float3(floor(enc255), residual.x * 16 + residual.y) / 255, shininess);
}

float4 StoreGBufferRT1(float3 diffuse, float3 specular)
{
	return float4(diffuse, dot(specular, float3(0.2126f, 0.7152f, 0.0722f)));
}

void StoreGBufferMRT(float3 normal, float3 specular, float shininess, float3 diffuse,
	out float4 mrt_0, out float4 mrt_1)
{
	mrt_0 = StoreGBufferRT0(normal, shininess);
	mrt_1 = StoreGBufferRT1(diffuse, specular);
}
//mrt0前三个通道的24bit（存了normal的两个分量）获取normal
float3 GetNormal(float4 mrt0)
{
	float nz = floor(mrt0.z * 255) / 16;
	mrt0.xy += float2(floor(nz) / 16, frac(nz)) / 255;
	float2 fenc = mrt0.xy * 4 - 2;
	float f = dot(fenc, fenc);
	float g = sqrt(1 - f / 4);
	float3 normal;
	normal.xy = fenc * g;
	normal.z = f / 2 - 1;
	return normal;
}

float GetShininess(float4 mrt0)
{
	return mrt0.w * 50.0f;
}

float3 GetDiffuse(float4 mrt1)
{
	return mrt1.xyz;
}

float GetSpecular(float4 mrt1)
{
	return mrt1.w;
}

