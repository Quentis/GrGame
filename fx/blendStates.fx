BlendState defaultBlender
{
};

BlendState additiveBlender
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = one;
	BlendOp = add;
	SrcBlendAlpha = one;
	DestBlendAlpha = one;
	BlendOpAlpha = add;
};

BlendState transparencyBlender
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = one;
	DestBlendAlpha = one;
	BlendOpAlpha = add;
};