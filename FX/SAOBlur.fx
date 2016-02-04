/** 
  \file SAO_blur.pix
  \author Morgan McGuire and Michael Mara, NVIDIA Research

  \brief 7-tap 1D cross-bilateral blur using a packed depth key

  DX11 HLSL port by Leonardo Zide, Treyarch
  
  Open Source under the "BSD" license: http://www.opensource.org/licenses/bsd-license.php

  Copyright (c) 2011-2012, NVIDIA
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Tunable Parameters:

/** Increase to make edges crisper. Decrease to reduce temporal flicker. */
#define EDGE_SHARPNESS     (0.5)

/** Step in 2-pixel intervals since we already blurred against neighbors in the
    first AO pass.  This constant can be increased while R decreases to improve
    performance at the expense of some dithering artifacts. 
    
    Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
    unobjectionable after shading was applied but eliminated most temporal incoherence
    from using small numbers of sample taps.
    */
#define SCALE               (1)

/** Filter radius in pixels. This will be multiplied by SCALE. */
#define R                   (8)



//////////////////////////////////////////////////////////////////////////////////////////////

/** Type of data to read from source.  This macro allows
    the same blur shader to be used on different kinds of input data. */
#define VALUE_TYPE        float

/** Swizzle to use to extract the channels of source. This macro allows
    the same blur shader to be used on different kinds of input data. */
#define VALUE_COMPONENTS   r

#define VALUE_IS_KEY       0

/** Channel encoding the bilateral key value (which must not be the same as VALUE_COMPONENTS) */
#define KEY_COMPONENTS     gb

// Gaussian coefficients
static const float gaussian[] = 
//	{ 0.356642, 0.239400, 0.072410, 0.009869 };
//	{ 0.398943, 0.241971, 0.053991, 0.004432, 0.000134 };  // stddev = 1.0
//	{ 0.153170, 0.144893, 0.122649, 0.092902, 0.062970 };  // stddev = 2.0
	{0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216, 0.000436,0.000067, 0.000008,0.000001};
//	{0.132981, 0.125794, 0.106483, 0.080657, 0.054670, 0.033159, 0.017997, 0.008741, 0.003799, 0.001477, 0.000514, 0.000160, 0.000045, 0.000011, 0.000002};
//	{ 0.111220, 0.107798, 0.098151, 0.083953, 0.067458, 0.050920, 0.036108 }; // stddev = 3.0

Texture2D<float4> gSource;
sampler TexSource = sampler_state
{
	Texture = <gSource>;
	MinFilter =	POINT;
	MagFilter =	POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler TexSourceL = sampler_state
{
	Texture = <gSource>;
	MinFilter =	LINEAR;
	MagFilter =	LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};


/** (1, 0) or (0, 1)*/
//float2 axis;
float2 gAxis;
float2 gRes;

#define  result         fragment.color.VALUE_COMPONENTS
#define  keyPassThrough fragment.color.KEY_COMPONENTS

struct PixelInput
{
	float4 position : POSITION;
	float2 texCoords : TEXCOORD0;
};

struct PixelOutput
{
	float4 color : COLOR;
};


PixelInput VS(float3 pos : POSITION, float2 tex : TEXCOORD)
{
	PixelInput vout;
	vout.position = float4(pos.xy, 1.0f, 1.0f);
	vout.texCoords = tex;
	return vout;
}


/** Returns a number on (0, 1) */
float unpackKey(float2 p)
{
	return p.x * (256.0 / 257.0) + p.y * (1.0 / 257.0);
}

PixelOutput PS(const PixelInput pixel, in float4 cpos : VPOS)
{
	PixelOutput fragment;
	fragment.color = 1;

	float2 ssC = pixel.texCoords  * gRes;//cpos.xy;//screen coord
	//float2 uv = (ssC) / gRes;
	//float4 temp =  tex2D(TexSource, uv);
	float4 temp = tex2D(TexSource, pixel.texCoords);
	keyPassThrough = temp.KEY_COMPONENTS;
	float key = unpackKey(keyPassThrough);
	float v = temp.VALUE_COMPONENTS;
	float sum = temp.VALUE_COMPONENTS;

	if (key == 1.0) { 
		// Sky pixel (if you aren't using depth keying, disable this test)
		result = sum;
		return fragment;
	}

	// Base weight for depth falloff.  Increase this for more blurriness,
	// decrease it for better edge discrimination
	float BASE = gaussian[0];
	float totalWeight = BASE;
	sum *= totalWeight;

	[unroll]
	for (int r = -R; r <= R; ++r) {
		// We already handled the zero case above.  This loop should be unrolled and the branch discarded
		if (r != 0) {
			temp = tex2D(TexSource, (ssC + gAxis * (r * SCALE)) / gRes);
			float tapKey = unpackKey(temp.KEY_COMPONENTS);

			// spatial domain: offset gaussian tap
			float weight = 0.3 + gaussian[abs(r)];

			// range domain (the "bilateral" weight). As depth difference increases, decrease weight.
			weight *= max(0.0, 1.0 - (2000.0 * EDGE_SHARPNESS) * abs(tapKey - key));
			//float weight = gaussian[abs(r)];
			sum += temp.VALUE_COMPONENTS * weight;
			totalWeight += weight;
		}
	}

	const float epsilon = 0.0001;
	result = sum / (totalWeight + epsilon);	

	return fragment;
}


technique SAOBlurTech
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

