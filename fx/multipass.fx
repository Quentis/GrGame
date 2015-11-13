#include <envmapped.fx>

Texture2D multipassTexture;

float4 psShow(VsosQuad input) : SV_Target
{
	return multipassTexture.Sample(linearSampler, 1 - sin(2 * 3.14 * input.tex) * sin(2 * 3.14 * input.tex));
}

technique11 show
{
	pass background
	{
		SetVertexShader(CompileShader(vs_5_0, vsQuad()));
		SetGeometryShader(NULL);
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, psShow()));
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}