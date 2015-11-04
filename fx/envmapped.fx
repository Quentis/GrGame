#include <textured.fx>

struct IaosQuad
{
	float4  pos: POSITION;
	float2  tex: TEXCOORD0;
};
struct VsosQuad
{
	float4 pos: SV_POSITION;
	float2 tex: TEXCOORD0;
	float3 viewDir: TEXCOORD1;
};

float3 eyePos;
TextureCube envTexture;
float4x4 viewDirMatrix;

VsosQuad vsQuad(IaosQuad input)
{
	VsosQuad output = (VsosQuad)0;
	output.pos = input.pos;
	float4 hWorldPosMinusEye = mul(input.pos, viewDirMatrix);
	hWorldPosMinusEye /= hWorldPosMinusEye.w;
	output.viewDir = hWorldPosMinusEye.xyz;
	output.tex = input.tex;
	return output;
}

float4 psBackground(VsosQuad input) : SV_Target
{
	return envTexture.Sample(linearSampler, input.viewDir);
}

float4 psEnvMapped(VsosTrafo input) : SV_Target
{
	float3 viewDir = normalize(input.worldPos - eyePos);
	float3 reflectionDir = reflect(viewDir, input.normal);
	return 0.5 * envTexture.Sample(linearSampler, reflectionDir) +
		   0.5 * kdTexture.Sample(linearSampler, input.tex);
}
technique11 envmapped
{
	pass envmapped
	{
		SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
		SetGeometryShader(NULL);
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, psEnvMapped()));
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
	pass background
	{
		SetVertexShader(CompileShader(vs_5_0, vsQuad()));
		SetGeometryShader(NULL);
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, psBackground()));
		SetDepthStencilState(defaultCompositor, 0);
		SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}