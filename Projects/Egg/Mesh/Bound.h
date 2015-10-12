#pragma once
#include "Mesh/Geometry.h"

namespace Egg { namespace Mesh
{
	class Geometry;

	class Bound
	{
		Geometry::P geometry;
		ID3D11InputLayout* inputLayout;

		Bound(Geometry::P geometry, ID3D11InputLayout* inputLayout);
	public:
		typedef boost::shared_ptr<Bound> P;
		static Bound::P create(Geometry::P geometry, ID3D11InputLayout* inputLayout) { return Bound::P(new Bound(geometry, inputLayout));}

		Geometry::P getGeometry(){return geometry;}
		ID3D11InputLayout* getInputLayout(){return inputLayout;}

		~Bound(void);

		void draw(ID3D11DeviceContext* context);
	};

}} // namespace Egg::Mesh
