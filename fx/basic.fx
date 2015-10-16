float4x4 modelMatrix;
float4x4 modelMatrixInverse;
float4x4 modelViewProjMatrix;

struct IaosTrafo
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct VsosTrafo
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

VsosTrafo vsTrafo(IaosTrafo input)
{
	VsosTrafo output = (VsosTrafo)0;

	output.pos = mul(input.pos, modelViewProjMatrix);
	output.worldPos = mul(input.pos, modelMatrix);
	output.normal = mul(modelMatrixInverse, float4(input.normal.xyz, 0.0));
	output.tex = input.tex;

	return output;
}

float4 psBasic(VsosTrafo input) : SV_Target
{
	return saturate(input.normal).y;
}

technique11 basic
{
	pass basic
	{
		SetVertexShader(CompileShader(
			vs_5_0, vsTrafo()));
		SetPixelShader(CompileShader(
			ps_5_0, psBasic()));
	}
}