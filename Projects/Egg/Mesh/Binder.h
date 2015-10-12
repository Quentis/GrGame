#pragma once
#include "d3dx11effect.h"
#include "Mesh/Geometry.h"
#include "Mesh/Bound.h"
#include "Mesh/Shaded.h"
#include "Mesh/Material.h"
#include <vector>

namespace Egg { namespace Mesh
{

	class Geometry;
	class Bound;

	class Binder
	{
		class InputConfiguration
		{
			friend class Mesh::Binder;

			const D3D11_INPUT_ELEMENT_DESC* elements;
			unsigned int nElements;
			const D3DX11_PASS_DESC& passDesc;

			ID3D11InputLayout* inputLayout;

			InputConfiguration(const D3DX11_PASS_DESC& passDesc, Mesh::Geometry::P geometry);

			/// Returns true if input signatures are identical and elements with shared semantics are also identical.
			bool isCompatible(const InputConfiguration& other) const;

			HRESULT createInputLayout(ID3D11Device* device);
		public:
			~InputConfiguration();
		};

		typedef std::vector<InputConfiguration*> InputConfigurationList;
		InputConfigurationList inputConfigurationList;

		ID3D11InputLayout* getCompatibleInputLayout(const D3DX11_PASS_DESC& passDesc, Mesh::Geometry::P geometry);

		ID3D11Device* device;
		Binder(ID3D11Device* device);
	public:
		typedef boost::shared_ptr<Binder> P;
		static Binder::P create(ID3D11Device* device) { return Binder::P(new Binder(device));}

		~Binder();

		Bound::P bind(const D3DX11_PASS_DESC& passDesc, Mesh::Geometry::P geometry);
		Shaded::P bindMaterial(Mesh::Material::P material, Mesh::Geometry::P geometry);
		void replaceMaterial(Mesh::Material::P material, Mesh::Shaded::P& shaded);

	};

}} // namespace Egg::Mesh