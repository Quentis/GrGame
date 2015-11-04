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