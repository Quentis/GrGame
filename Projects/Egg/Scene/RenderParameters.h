#pragma once
#include "d3dx11effect.h"
#include "Cam/Base.h"
#include "Mesh/Mien.h"

#pragma warning( disable : 4512 ) // 'assignment operator could not be generated' because of const member

/// Structure passed as parameter to render calls.
namespace Egg { namespace Scene
{
	class RenderParameters
	{
	public:
		/// D3D device reference.
		ID3D11DeviceContext* context;
		/// D3DX effect reference.
		ID3DX11Effect* effect;
		/// Pointer to uniform modelMatrix variable
		ID3DX11EffectMatrixVariable* modelMatrix;
		/// Pointer to uniform modelMatrixInverse variable
		ID3DX11EffectMatrixVariable* modelMatrixInverse;
		/// Pointer to uniform modelViewProjMatrix variable
		ID3DX11EffectMatrixVariable* modelViewProjMatrix;
		/// Pointer to uniform viewDirMatrix variable
		ID3DX11EffectMatrixVariable* viewDirMatrix;
		/// Pointer to uniform eyePos variable
		ID3DX11EffectVectorVariable* eyePos;

		/// Camera.
		Cam::Base::P camera;
		/// Identifier to pick appropriate FlipMesh Mien.
		Mesh::Mien mien;

	};
}}

#pragma warning( default : 4512 ) 
