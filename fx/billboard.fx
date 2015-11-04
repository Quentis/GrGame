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
	float opacity : OPACITY;
};

float billboardWidth = 0.1;
float billboardHeight = 0.1;

[maxvertexcount(4)]
void gsBillboard(
	point VsosBillboard input[1],
	inout TriangleStream<GsosBillboard> stream) {
	float4 hndcPos = mul(float4(input[0].pos, 1), modelViewProjMatrix);
	float s = input[0].age / input[0].lifespan;

	GsosBillboard output;
	output.pos = hndcPos;
	output.pos.x += billboardWidth * s;
	output.pos.y += billboardHeight * s;
	output.tex = float2(1, 0);
	output.opacity = 1 - abs(s * 2 - 1);
	stream.Append(output);

	output.pos = hndcPos;
	output.pos.x += billboardWidth * s;
	output.pos.y -= billboardHeight * s;
	output.tex = float2(1, 1);
	output.opacity = 1 - abs(s * 2 - 1);
	stream.Append(output);

	output.pos = hndcPos;
	output.pos.x -= billboardWidth * s;
	output.pos.y += billboardHeight * s;
	output.tex = float2(0, 0);
	output.opacity = 1 - abs(s * 2 - 1);
	stream.Append(output);

	output.pos = hndcPos;
	output.pos.x -= billboardWidth * s;
	output.pos.y -= billboardHeight * s;
	output.tex = float2(0, 1);
	output.opacity = 1 - abs(s * 2 - 1);
	stream.Append(output);
}

//float4 psFire(GsosBillboard input) : SV_Target
//{
//	return input.tex.xyyy;
//}

Texture2D billboardTexture;

//float4 psFire(GsosBillboard input) : SV_Target
//{
//	return billboardTexture.Sample(linearSampler, input.tex.xy);
//}
float4 psFire(GsosBillboard input) : SV_Target
{
	float4 color = billboardTexture.Sample(linearSampler, input.tex.xy);
	color.rgb = float3(color.a, pow(color.a, 4), pow(color.a, 10));
	color.a *= input.opacity;
	return color;
}

technique11 billboard {
	pass fire {
		SetVertexShader(CompileShader(vs_5_0, vsBillboard()));
		SetGeometryShader(CompileShader(gs_5_0, gsBillboard()));
		SetRasterizerState(defaultRasterizer);
		SetPixelShader(CompileShader(ps_5_0, psFire()));
		SetDepthStencilState(noDepthWriteCompositor, 0);
		SetBlendState(transparencyBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}