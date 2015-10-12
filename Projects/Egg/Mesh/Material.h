#pragma once
#include "d3dx11effect.h"
#include <boost/shared_ptr.hpp>
#include <map>

namespace Egg { namespace Mesh
{
	class Material
	{
		class Flags
		{
			unsigned int flags;
			Flags(unsigned int flag):flags(flag){}
		public:
			Flags operator|(Flags o){ return flags | o.flags; }
			static const Flags NONE;
			static const Flags KEEP_UAV_COUNTERS;
		};

		ID3DX11EffectPass* pass;
		unsigned int flags;

		typedef std::map<ID3DX11EffectVariable*, void*>		EffectVariableSettings;
		EffectVariableSettings effectVariableSettings;

		Material(ID3DX11EffectPass* pass, unsigned int flags):pass(pass),flags(flags)
		{
		}
	public:
		typedef boost::shared_ptr<Material> P;
		static Material::P create(ID3DX11EffectPass* pass, unsigned int flags) { return Material::P(new Material(pass, flags)); }

		Material::P clone();

		~Material();

		void setPass(ID3DX11EffectPass* pass, unsigned int flags)
		{
			this->pass = pass;
			this->flags = flags;
		}

		void saveVariable(ID3DX11EffectVariable* variable);
		
		void apply(ID3D11DeviceContext* context);

		void getPassDesc(D3DX11_PASS_DESC& passDesc) { pass->GetDesc(&passDesc);}


		class Setter{
			Material& material;
			ID3DX11EffectVariable* variable;
		public:
			Setter(Material& material, ID3DX11EffectVariable* variable)
				:material(material),variable(variable) {}
			~Setter() { material.saveVariable(variable); }
			ID3DX11EffectVariable* operator->(){ return variable;}
		};
		Setter set(ID3DX11EffectVariable* variable)
			{ return Setter(*this, variable); }

	};

}} // namespace Egg::Mesh
