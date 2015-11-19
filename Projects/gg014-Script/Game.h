#pragma once

#include "App/App.h"
#include "Scene/ManagerApp.h"
#include "Mesh/Shaded.h"
#include "Mesh/Binder.h"
#include "Cam/FirstPerson.h"
#include "Particle.h"
#include <vector>

class Game : public Egg::Scene::ManagerApp
{
private:

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

