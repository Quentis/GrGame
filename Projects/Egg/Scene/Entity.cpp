#include "DXUT.h"
#include "Scene/Entity.h"

using namespace Egg; 

void Scene::Entity::render(const RenderParameters& renderParameters)
{
	using namespace Egg::Math;

	renderParameters.modelMatrix->SetMatrix((float*)&getModelMatrix());
	renderParameters.modelMatrixInverse->SetMatrix((float*)&getModelMatrixInverse());

	float4x4 modelViewProjMatrix = getModelMatrix() * renderParameters.camera->getViewMatrix() * renderParameters.camera->getProjMatrix();
	renderParameters.modelViewProjMatrix->SetMatrix((float*)&modelViewProjMatrix);

	multiMesh->draw(renderParameters.context, renderParameters.mien);
}
