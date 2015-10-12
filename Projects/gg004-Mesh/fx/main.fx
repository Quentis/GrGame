float4 vsIdle(float4 pos : POSITION) : SV_Position
{
	return pos;
}
float4 psIdle() : SV_Target
{
	return float4(0, 1, 0, 1);
}
technique11 idle
{
	pass idle
	{
		SetVertexShader(
			CompileShader(vs_5_0, vsIdle()));
		SetPixelShader(
			CompileShader(ps_5_0, psIdle()));
	}
}