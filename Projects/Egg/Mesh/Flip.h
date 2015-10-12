#pragma once
#include "Mesh/Shaded.h"
#include "Mesh/Mien.h"
#include "Mesh/Geometry.h"
#include "Mesh/Binder.h"

namespace Egg { namespace Mesh
{
	class Flip
	{
		typedef std::map<Mien, Shaded::P> MienShadedMap;
		MienShadedMap mienShadedMap;

		Flip();
	public:
		typedef boost::shared_ptr<Flip> P;
		static Flip::P create() { return Flip::P(new Flip());}

		~Flip(void);

		void add(Mien mien, Shaded::P shaded);
		void add(Mien mien, ID3DX11EffectPass* pass, unsigned int flags,  Geometry::P geometry, Binder::P binder);

		Shaded::P& getShaded(Mien mien)
		{
			return mienShadedMap[mien];
		}

		void draw(ID3D11DeviceContext* context, Mien mien);
	};

}} // namespace Egg::Mesh
