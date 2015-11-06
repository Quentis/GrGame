#pragma once

#include "App/App.h"
#include "Mesh/Shaded.h"
#include "Mesh/Binder.h"
#include "Cam/FirstPerson.h"
#include "Particle.h"
#include <vector>

class Game : public Egg::App
{
private:
	Egg::Mesh::Binder::P binder;
	Egg::Mesh::Shaded::P shadedMesh;
	Egg::Cam::FirstPerson::P firstPersonCam;
	ID3D11ShaderResourceView* kdSrv;
	ID3D11ShaderResourceView* envSrv;
	Egg::Mesh::Shaded::P backgroundQuad;

	std::vector<Particle> particles;
	Egg::Mesh::Shaded::P fireBillboardSet;
	ID3D11ShaderResourceView* billboardSrv;

	void setTransformation();

public:
	Game() = delete;
	Game(ID3D11Device* device);
	~Game();

	virtual std::string getMainEffectFilename() override { return "main.fx"; }

	virtual HRESULT createResources() override;

	virtual HRESULT releaseResources() override;

	virtual void render(ID3D11DeviceContext* context) override;

	virtual void animate(double dt, double t) override;

	virtual bool processMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual HRESULT createSwapChainResources() override;

	virtual HRESULT releaseSwapChainResources() override;

};

