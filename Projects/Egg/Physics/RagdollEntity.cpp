#include "DXUT.h"
#include "Physics/RagdollEntity.h"
#include "Physics/PhysicsMathConversions.h"

using namespace Egg;
using namespace Egg::Physics;
using namespace physx;

RagdollEntity::RagdollEntity(Egg::Math::float3 position, Egg::Math::float4 orientation, Mesh::Multi::P multiMesh)
	:Scene::Entity(multiMesh)
{
}

RagdollEntity::~RagdollEntity()
{}

Math::float4x4 RagdollEntity::getModelMatrix()
{
	return boneEntities.at(0)->getModelMatrix();
}

Math::float4x4 RagdollEntity::getModelMatrixInverse()
{
	return boneEntities.at(0)->getModelMatrixInverse();
}

Math::float3 RagdollEntity::getPosition()
{
	return boneEntities.at(0)->getPosition();
}

Math::float4 RagdollEntity::getOrientation()
{
	return boneEntities.at(0)->getOrientation();
}

Math::float4x4 RagdollEntity::getRotationMatrix()
{
	return boneEntities.at(0)->getRotationMatrix();
}

Math::float4x4 RagdollEntity::getRotationMatrixInverse()
{
	return boneEntities.at(0)->getRotationMatrixInverse();
}

void RagdollEntity::addBoneEntity(PhysicsEntity::P boneEntity)
{
	boneEntities.push_back(boneEntity);
	riggingBoneTransforms.push_back(boneEntity->getModelMatrixInverse());
}

void RagdollEntity::render(const Egg::Scene::RenderParameters& renderParameters)
{
	using namespace Egg::Math;

	float4x4 boneTransforms[128];
	for(int iBone=0; iBone < boneEntities.size(); iBone++)
		boneTransforms[iBone] = riggingBoneTransforms.at(iBone) * boneEntities.at(iBone)->getModelMatrix();

	ID3DX11EffectVariable* bonesVariable = renderParameters.effect->GetVariableByName("bones");
//	bonesVariable->SetRawValue(boneTransforms, 0, boneEntities.size() * sizeof(float4x4));
	bonesVariable->AsMatrix()->SetMatrixArray((float*)boneTransforms, 0, 128 /* boneEntities.size()*/);

	renderParameters.modelMatrix->SetMatrix((float*)&float4x4::identity);
	renderParameters.modelMatrixInverse->SetMatrix((float*)&float4x4::identity);

	float4x4 modelViewProjMatrix = renderParameters.camera->getViewMatrix() * renderParameters.camera->getProjMatrix();
	renderParameters.modelViewProjMatrix->SetMatrix((float*)&modelViewProjMatrix);

	multiMesh->draw(renderParameters.context, renderParameters.mien);
}

