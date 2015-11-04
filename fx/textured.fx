#include <basic.fx>

Texture2D kdTexture;

SamplerState linearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 psTextured(VsosTrafo input) : SV_Target
{
	return abs(saturate(input.normal).y) *
	kdTexture.Sample(linearSampler, input.tex);
}
technique11 textured
{
	pass textured
	{
		SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, psTextured()));
	}
}