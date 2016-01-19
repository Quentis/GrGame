#pragma once
#include "Control/ControlApp.h"

class Game : public Egg::Control::ControlApp
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

