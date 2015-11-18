#pragma once
#include <boost/shared_ptr.hpp>
#include "Sas/SasApp.h"
#include "Scene/Directory.h"
#include "Scene/Entity.h"
#include "Scene/RenderParameters.h"
#include "Scene/ControlParameters.h"
#include "Mesh/Indexed.h"
#include "Mesh/Multi.h"
#include "Cam/Base.h"
#include <assimp/importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postProcess.h> // Post processing flags

namespace Egg { namespace Scene
{
	/// Application class with scene management
	class ManagerApp : public Egg::Sas::SasApp
	{
	protected:
		/// Mesh binder
		Egg::Mesh::Binder::P binder;

		/// Called from loadMultiMesh(). The contents of the file are imported as aiMeshes. From every aiMesh, a Mesh::Indexed is created.
		/// Then this function is called to create all necessary ShadedMesh instances, and add them to the Mesh::Flip.
		/// This can be overridden to support multiple Mien-s.
		virtual void addDefaultShadedMeshes(Mesh::Flip::P flipMesh, Mesh::Indexed::P indexedMesh, aiMaterial* assMaterial);
		/// Imports a file with AssImp and creates a Mesh::Multi. Uses addDefaultShadedMeshes() to create the ShadedMesh instances.
		Mesh::Multi::P loadMultiMesh(const std::string& filename, unsigned int flags, const std::string& alias = "");

		/// Master structure containing everything that should be passed to Entity::render().
		RenderParameters renderParameters;
		/// Master structure containing everything that should be passed to Entity::control().
		ControlParameters controlParameters;

		typedef std::map<std::string, Mesh::Mien> MienDirectory;
		/// Directory containing all rendering Miens used.  Addressed by mien name (e.g. "basic").
		MienDirectory miens;
		/// Directory containing all rendering SRVs used. Addressed by file path.
		DxResourceDirectory<ID3D11ShaderResourceView> shaderResourceViews;
		/// Directory containing named Mesh::Indexed instances. Addressed by name.
		Directory<Mesh::Indexed> indexedMeshes;
		/// Directory containing named Mesh::Multi instances. Addressed by name.
		Directory<Mesh::Multi> multiMeshes;
		/// Directory containing named Mesh::Material instances. Addressed by name.
		Directory<Mesh::Material> materials;
		/// Directory containing all entities. Addressed by name.
		Directory<Entity> entities;
		/// Directory containing all cameras. Addressed by name.
		Directory<Cam::Base> cameras;
		/// Iterator to current camera in cameras. Must be updated when cameras is changed.
		Directory<Cam::Base>::iterator currentCamera;

		/// Returns the SRV to a texture resource, creating it from file if not yet loaded.
		ID3D11ShaderResourceView* loadSrv(const std::string& filename, const std::string& alias = "");
		/// Returns a Mesh::Indexed, creating it from file if not yet loaded.
		Mesh::Indexed::P loadIndexedMesh(const std::string& filename, const std::string& alias = "");
		/// Returns a Mien, creating it if it does not exist yet.
		Mesh::Mien getMien(const std::string& mienName);

	public:
		ManagerApp(ID3D11Device* device):SasApp(device){currentCamera=cameras.end();}
		virtual HRESULT createResources();
		virtual HRESULT releaseResources();

		virtual void animate(double dt, double t);
		virtual bool processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void render(ID3D11DeviceContext* context);

	};

}}