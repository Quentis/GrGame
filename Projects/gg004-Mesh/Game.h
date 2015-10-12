#pragma once

#include "App/App.h"
#include "Mesh/Shaded.h"
#include "Mesh/Binder.h"

class Game : public Egg::App
{
private:
	Egg::Mesh::Binder::P binder;
	Egg::Mesh::Shaded::P shadedMesh;

public:
	Game() = delete;
	Game(ID3D11Device* device);
	~Game();

	virtual std::string getMainEffectFilename() override { return "main.fx"; }

	virtual HRESULT createResources() override;

	virtual HRESULT releaseResources() override;

	virtual void render(ID3D11DeviceContext* context) override;


};

