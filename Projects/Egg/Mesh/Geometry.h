#pragma once
#include <boost/shared_ptr.hpp>

namespace Egg { namespace Mesh
{
	class Geometry
	{
	public:
		typedef boost::shared_ptr<Geometry> P;

		virtual ~Geometry(void)
		{
		}

		virtual void getElements(const D3D11_INPUT_ELEMENT_DESC*& elements, unsigned int& nElements)=0;

		virtual void draw(ID3D11DeviceContext* context)=0;
		virtual ID3D11Buffer* getPrimaryBuffer()=0;
	};

}} // namespace Egg::Mesh