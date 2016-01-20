Texture2D normalTexture;
Texture2D bumpTexture;

float4 psNormalMapped(VsosTrafo input) : SV_TARGET {
	float3 normal = normalize(input.normal);

	float2 dtdx = ddx(input.tex);
	float2 dtdy = ddy(input.tex);
	float3 dpdx = ddx(input.worldPos);
	float3 dpdy = ddy(input.worldPos);

	float3 sTangent = (dpdx * dtdy.y - dpdy * dtdx.y);
	float3 sBinormal = (dpdy * dtdx.x - dpdx * dtdy.x) ;
	float3 N = normalize(cross(sBinormal, sTangent));
	float3 B = normalize(cross(input.normal, sTangent));
	float3 T = normalize(cross(B, input.normal));
	//return abs(N.xyzz); 
	float3 tangentNormal = normalize(normalTexture.Sample(linearSampler, input.tex).xzy - float3(0.5, 0.5, 0.5));
	float3 worldNormal = B * tangentNormal.z + normal * tangentNormal.y + T * tangentNormal.x;
	return abs(worldNormal.y) * kdTexture.Sample(linearSampler, input.tex);
}

float4 psParallaxMapped(VsosTrafo input) : SV_TARGET{
	float3 normal = normalize(input.normal);

	float2 dtdx = ddx(input.tex);
	float2 dtdy = ddy(input.tex);
	float3 dpdx = ddx(input.worldPos);
	float3 dpdy = ddy(input.worldPos);

	float3 sTangent = (dpdx * dtdy.y - dpdy * dtdx.y);
	float3 sBinormal = (dpdy * dtdx.x - dpdx * dtdy.x);
	float3 N = normalize(cross(sBinormal, sTangent));
	float3 B = normalize(cross(input.normal, sTangent));
	float3 T = normalize(cross(B, input.normal));
	
	float bump_height = 0.1;
	float bump = bumpTexture.Sample(linearSampler, input.tex);
	float3 viewDir = normalize(eyePos - input.worldPos);
		viewDir = float3(dot(viewDir, T), dot(viewDir, B), dot(viewDir, normal));
	float2 texOffset = bump_height * viewDir.xy
		// / viewDir.z 
		* bump;
	input.tex += texOffset;

	float3 tangentNormal = normalize(normalTexture.Sample(linearSampler, input.tex).xzy - float3(0.5, 0.5, 0.5));

	float3 worldNormal = B * tangentNormal.z + normal * tangentNormal.y + T * tangentNormal.x;

	return abs(worldNormal.y) * kdTexture.Sample(linearSampler, input.tex);
}

float4 psReliefMapped(VsosTrafo input) : SV_TARGET{
	float3 normal = normalize(input.normal);
	float2 dtdx = ddx(input.tex);
	float2 dtdy = ddy(input.tex);
	float3 dpdx = ddx(input.worldPos);
	float3 dpdy = ddy(input.worldPos);
	float3 sTangent = (dpdx * dtdy.y - dpdy * dtdx.y);
	float3 sBinormal = (dpdy * dtdx.x - dpdx * dtdy.x);
	float3 N = normalize(cross(sBinormal, sTangent));
	float3 B = normalize(cross(input.normal, sTangent));
	float3 T = normalize(cross(B, input.normal));

	float bump_height = 0.2;
	float bump = bumpTexture.Sample(linearSampler, input.tex);
	float3 viewDir = normalize(eyePos - input.worldPos);
		viewDir = float3(dot(viewDir, T), dot(viewDir, B), dot(viewDir, normal));

	float3 sRange = -viewDir * bump_height / viewDir.z * 0.5;
		float3 sPos = float3(input.tex, 0) - sRange;

		for (int i = 0; i<6; i++)
		{
			float bump = bumpTexture.Sample(linearSampler, sPos.xy);
			sRange *= 0.5;
			if (sPos.z > bump * bump_height) // If outside
				sPos += sRange; // Move forward
			else
				sPos -= sRange; // Move backward
		}
	sPos -= 4 * sRange;
	input.tex = sPos.xy;

	float3 tangentNormal = normalize(tex2D(normalMapSampler, input.tex).xzy - float3(0.5, 0.5, 0.5));

	float3 worldNormal = B * tangentNormal.z + normal * tangentNormal.y + T * tangentNormal.x;

	return abs(worldNormal.y) * tex2D(kdMapSampler, input.tex);
}


technique11 displacement
{
  pass normalMapped
  {
        SetVertexShader ( CompileShader( vs_5_0, vsTrafo() ) );
        SetGeometryShader( NULL );
        SetRasterizerState( defaultRasterizer );
        SetPixelShader( CompileShader( ps_5_0, psNormalMapped() ) );
        SetDepthStencilState( defaultCompositor, 0 );
        SetBlendState( defaultBlender, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF  );
  }
  pass parallaxMapped
  {
	  SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
	  SetGeometryShader(NULL);
	  SetRasterizerState(defaultRasterizer);
	  SetPixelShader(CompileShader(ps_5_0, psParallaxMapped()));
	  SetDepthStencilState(defaultCompositor, 0);
	  SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
  }
  pass reliefMapped
  {
	  SetVertexShader(CompileShader(vs_5_0, vsTrafo()));
	  SetGeometryShader(NULL);
	  SetRasterizerState(defaultRasterizer);
	  SetPixelShader(CompileShader(ps_5_0, psReliefMapped()));
	  SetDepthStencilState(defaultCompositor, 0);
	  SetBlendState(defaultBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
  }
}
