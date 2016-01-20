RasterizerState defaultRasterizer
{
};

RasterizerState noCullRasterizer
{
	CullMode = none;
	FillMode = solid;
};

RasterizerState backfaceRasterizer
{
	CullMode = front;
	FillMode = solid;
};

RasterizerState wireframeRasterizer
{
	CullMode = none;
	FillMode = wireFrame;
};

RasterizerState shadowRasterizer
{
	// If the depth buffer currently bound to the output-merger stage has a UNORM
	// format or no depth buffer is bound the bias value is calculated like this:
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	// where r is the minimum representable value > 0 in the depth-buffer format
	// converted to float32. For a 24-bit depth buffer, r = 1 / 2^24.
	DepthBias = 100000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};