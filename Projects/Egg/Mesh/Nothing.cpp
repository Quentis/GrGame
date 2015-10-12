#include "DXUT.h"
#include "Mesh/Nothing.h"
#include "App/ThrowOnFail.h"

using namespace Egg;

Mesh::Nothing::Nothing(unsigned int nVertices, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	this->topology = topology;
	this->nVertices = nVertices;
}

Mesh::Nothing::~Nothing(void)
{
}

void Mesh::Nothing::getElements(const D3D11_INPUT_ELEMENT_DESC*& elements, unsigned int& nElements)
{
	elements = NULL;
	nElements = 0;
}

void Mesh::Nothing::draw(ID3D11DeviceContext* context)
{
	context->IASetPrimitiveTopology(topology);
	context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	context->Draw(nVertices, 0);
}
