#include "DXUT.h"
#include "Mesh/Shaded.h"

using namespace Egg;

void Mesh::Shaded::draw(ID3D11DeviceContext* context)
{
	material->apply(context);
	bound->draw(context);
}