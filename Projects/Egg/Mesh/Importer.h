#pragma once

#include "Mesh/Indexed.h"
#include "Mesh/Multi.h"
#include "Mesh/Binder.h"
#include "Mesh/Mien.h"
#include "d3dx11effect.h"

struct aiMesh;

namespace Egg { namespace Mesh
{
	class Geometry;

	class Importer
	{
	public:
		static Mesh::Indexed::P fromAiMesh(ID3D11Device* device, aiMesh* assMesh);
	};

}} // namespace Egg::Mesh