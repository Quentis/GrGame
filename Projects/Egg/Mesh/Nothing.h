#pragma once
#include "Mesh/Geometry.h"
#include <vector>

namespace Egg { namespace Mesh
{

	class Nothing :
		public Mesh::Geometry
	{
		unsigned int nVertices;
		D3D11_PRIMITIVE_TOPOLOGY topology;
		Nothing(unsigned int nVertices, D3D11_PRIMITIVE_TOPOLOGY topology);
	public:
		typedef boost::shared_ptr<Nothing> P;
		typedef std::vector< Nothing::P > A;
		static Nothing::P create(unsigned int nVertices, D3D11_PRIMITIVE_TOPOLOGY topology) { return Nothing::P(new Nothing(nVertices, topology));}
		~Nothing(void);

		void getElements(const D3D11_INPUT_ELEMENT_DESC*& elements, unsigned int& nElements);
		unsigned int getElementCount() {return 0;}

		void draw(ID3D11DeviceContext* context);
		ID3D11Buffer* getPrimaryBuffer(){return NULL;}

		unsigned int getStride() {return 0;}

	};


}} // namespace Egg::Mesh