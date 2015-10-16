#include <basic.fx>

float3 lightPos = float3(1, 1, 1);
float3 lightPower = float3(50, 50, 50);
float3 kd = float3(1, 0, 1); //lila

float4 psDiffuse(VsosTrafo input) : SV_Target{
	float3 normal = normalize(input.normal);
	float3 toLight = lightPos - input.worldPos.xyz;
	float3 lightDist2 = dot(toLight, toLight);
		float3 lightDir = normalize(toLight);
		return float4(
		kd * saturate(dot(lightDir, normal)) * lightPower /
		(4 * 3.14 * lightDist2), 1);
}

float3 eyePos;
float3 ks = float3(10, 10, 10);
float n = 10;

float4 psSpecular(VsosTrafo input) : SV_Target{
	float3 normal = normalize(input.normal);
	float3 toLight = lightPos - input.worldPos.xyz;
	float3 lightDist2 = dot(toLight, toLight);
	float3 lightDir = normalize(toLight);
	float3 toEye = eyePos - input.worldPos.xyz;
	float3 viewDir = normalize(toEye);
	return float4(
	(kd * saturate(dot(lightDir, normal))
	+ ks * pow(
	saturate(dot(reflect(-viewDir, normal),	lightDir)), n))	* lightPower
	/ (4 * 3.14 * lightDist2), 1);
}

technique11 shaded
{
	pass diffuse
	{
		SetVertexShader(CompileShader(
			vs_5_0, vsTrafo()));
		SetPixelShader(CompileShader(
			ps_5_0, psDiffuse()));
	}
	pass specular {
		SetVertexShader(CompileShader(
			vs_5_0, vsTrafo()));
		SetPixelShader(CompileShader(ps_5_0,
			psSpecular()));
	}
}