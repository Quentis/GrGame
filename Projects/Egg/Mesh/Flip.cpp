#include "DXUT.h"
#include "Mesh/Flip.h"
#include "App/ThrowOnFail.h"

using namespace Egg;

Mesh::Flip::Flip()
{
}

Mesh::Flip::~Flip()
{
}

void Mesh::Flip::add(Mien mien, Shaded::P shaded)
{
	MienShadedMap::iterator i = mienShadedMap.find(mien);
	if(i != mienShadedMap.end())
		mienShadedMap.erase(i);
	mienShadedMap[mien] = shaded;
}

void Mesh::Flip::add(Mien mien, ID3DX11EffectPass* pass, unsigned int flags, Geometry::P geometry, Binder::P binder)
{
	if(!pass->IsValid())
		throw HrException(E_FAIL, "Effect pass not found.", __FILE__, __LINE__);

	D3DX11_PASS_DESC passDesc;
	pass->GetDesc(&passDesc);
	//todo: id invalid thtow
	add(mien,
		Mesh::Shaded::create(
			binder->bind(passDesc, geometry),
			Mesh::Material::create(pass, flags)));
}

void Mesh::Flip::draw(ID3D11DeviceContext* context, Mien mien)
{
	MienShadedMap::iterator i = mienShadedMap.find(mien);
	if(i != mienShadedMap.end())
	{
		i->second->draw(context);
	}
	else
	{
		// warning. could be intentional
	}
}