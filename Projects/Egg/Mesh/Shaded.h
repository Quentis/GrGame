#pragma once
#include "Mesh/Bound.h"
#include "Mesh/Material.h"

namespace Egg { namespace Mesh
{
	class Shaded
	{
		Bound::P bound;
		Material::P material;

		Shaded(Bound::P bound, Material::P material):bound(bound), material(material){}
	public:
		typedef boost::shared_ptr<Shaded> P;
		static Shaded::P create(Bound::P bound, Material::P material) { return Shaded::P(new Shaded(bound, material));}

		Bound::P getBound(){return bound;}
		Material::P getMaterial(){return material;}
		Geometry::P getGeometry(){return bound->getGeometry();}
		ID3D11InputLayout* getInputLayout(){return bound->getInputLayout();}

		void draw(ID3D11DeviceContext* context);
	};

}} // namespace Egg::Mesh
