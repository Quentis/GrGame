struct IaosBillboard
{
	float3 pos : POSITION;
	float lifespan : LIFESPAN;
	float age : AGE;
};
typedef IaosBillboard VsosBillboard;

VsosBillboard vsBillboard(IaosBillboard input)
{
	return input;
}

struct GsosBillboard
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD;
};

float billboardWidth = 0.1;
float billboardHeight = 0.1;

[maxvertexcount(4)]
void gsBillboard(
	point VsosBillboard input[1],
	inout TriangleStream<GsosBillboard> stream) {
	float4 hndcPos = mul(float4(input[0].pos, 1), modelViewProjMatrix);
	GsosBillboard output;
	output.pos = hndcPos;
	output.pos.x += billboardWidth;
	output.pos.y += billboardHeight;
	output.tex = float2(1, 0);
	stream.Append(output);
	output.pos = hndcPos;
	output.pos.x += billboardWidth;
	output.pos.y -= billboardHeight;
	output.tex = float2(1, 1);
	stream.Append(output);
	output.pos = hndcPos;
	output.pos.x -= billboardWidth;
	output.pos.y += billboardHeight;
	output.tex = float2(0, 0);
	stream.Append(output);
	output.pos = hndcPos;
	output.pos.x -= billboardWidth;
	output.pos.y -= billboardHeight;
	output.tex = float2(0, 1);
	stream.Append(output);
}

//float4 psFire(GsosBillboard input) : SV_Target
//{
//	return input.tex.xyyy;
//}

Texture2D billboardTexture;

float4 psFire(GsosBillboard input) : SV_Target
{
	return billboardTexture.Sample(linearSampler, input.tex.xy);
}

technique11 billboard {
	pass fire {
		SetVertexShader(CompileShader(vs_5_0, vsBillboard()));
		SetGeometryShader(CompileShader(gs_5_0, gsBillboard()));
		SetPixelShader(CompileShader(ps_5_0, psFire()));
	}
}