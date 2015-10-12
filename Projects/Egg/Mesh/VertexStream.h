#pragma once
#include "Mesh/Geometry.h"
#include <map>
#include <vector>

namespace Egg { namespace Mesh
{

	struct VertexStreamDesc
	{
		static const D3D11_INPUT_ELEMENT_DESC defaultElements[2];
		static const float defaultData[20];

		VertexStreamDesc():elements(defaultElements),nElements(2),vertexStride(5*sizeof(float)), nVertices(4), vertexData((void*)defaultData), usage(D3D11_USAGE_IMMUTABLE), bindFlags(D3D11_BIND_VERTEX_BUFFER), cpuAccessFlags(0), miscFlags(0) {}

		const D3D11_INPUT_ELEMENT_DESC* elements;
		unsigned int nElements;
		unsigned int vertexStride;
		unsigned int nVertices;
		void* vertexData;
		D3D11_USAGE usage;
		UINT bindFlags;
		UINT cpuAccessFlags;
		UINT miscFlags;
	};

	class VertexStream :
		public Mesh::Geometry
	{
		ID3D11Buffer* vertexBuffer;
		D3D11_INPUT_ELEMENT_DESC* elements;
		unsigned int nElements;
		unsigned int vertexStride;
		unsigned int nVertices;
		VertexStream(ID3D11Device* device, VertexStreamDesc& desc);
	public:
		typedef boost::shared_ptr<VertexStream> P;
		typedef std::vector< VertexStream::P > A;
		static VertexStream::P create(ID3D11Device* device, VertexStreamDesc& desc) { return VertexStream::P(new VertexStream(device, desc));}
		~VertexStream(void);

		void getElements(const D3D11_INPUT_ELEMENT_DESC*& elements, unsigned int& nElements);
		unsigned int getElementCount() {return nElements;}

		void draw(ID3D11DeviceContext* context);

		unsigned int getStride() {return vertexStride;}
		ID3D11Buffer* getBuffer() {return vertexBuffer;}
		ID3D11Buffer* getPrimaryBuffer() {return vertexBuffer;}
	};


}} // namespace Egg::Mesh