#include <basic.fx>

cbuffer cbPerFrame
{
	float4x4 gLightWVP;
};
// Nonnumeric values cannot be added to a cbuffer.

Texture2D gDiffuseMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VertexOut vsShadowMapBuilder(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	return vout;
}

// This is only used for alpha cut out geometry, so that shadows
// show up correctly. Geometry that does not need to sample a
// texture can use a NULL pixel shader for depth pass.
void PS(VertexOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);
		// Don′t write transparent pixels to the shadow map.
		clip(diffuse.a - 0.15f);
}

technique11 ShadowMapBuilderTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsShadowMapBuilder()));
		SetGeometryShader(NULL);
		SetRasterizerState(shadowRasterizer);
		SetPixelShader(NULL);
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}
technique11 ShadowMapBuilderAlphaClipTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsShadowMapBuilder()));
		SetGeometryShader(NULL);
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}