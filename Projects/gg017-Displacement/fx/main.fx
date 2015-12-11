#include <envmapped.fx>
#include <billboard.fx>
#include "displacement.fx"


VsosQuad vsMirror(IaosQuad input)
{
	VsosQuad output = (VsosQuad)0;
	output.pos = input.pos;
	output.pos.xy *= 0.3;
	output.pos.y += 0.5;
	output.tex = input.tex;

	float4 hWorldPosMinusEye = mul(input.pos, viewDirMatrix);
	output.viewDir = hWorldPosMinusEye.xyz;
	return output;
}

Texture2D mirrorTexture;

float4 psMirror(VsosQuad input) : SV_Target0
{
	return mirrorTexture.Sample(linearSampler, input.tex);
}

technique11 mirror {
	pass mirror {
		SetVertexShader ( CompileShader( vs_5_0, vsMirror() ) );
		SetGeometryShader ( NULL );
		SetRasterizerState( defaultRasterizer );
		SetPixelShader( CompileShader( ps_5_0, psMirror() ) );
		SetDepthStencilState( noDepthTestCompositor, 0 );
		SetBlendState( defaultBlender, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
