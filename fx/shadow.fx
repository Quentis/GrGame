#include <shaded.fx>

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
void psShadowMapBuilder(VertexOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);
		// Don′t write transparent pixels to the shadow map.
		clip(diffuse.a - 0.15f);
}

technique11 ShadowMapBuilder
{
	pass NormalPass
	{
		SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
		SetGeometryShader(NULL);
		SetRasterizerState(shadowRasterizer);
		SetPixelShader(NULL);
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass AlphaClipPass
	{
		SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
		SetGeometryShader(NULL);
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}