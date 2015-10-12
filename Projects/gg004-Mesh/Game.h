#pragma once

#include "App\App.h"

class Game : public Egg::App
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11InputLayout* inputLayout;

public:
	Game() = delete;
	Game(ID3D11Device* device);
	~Game();

	virtual std::string getMainEffectFilename() override { return "main.fx"; }

	virtual HRESULT createResources() override;

	virtual HRESULT releaseResources() override;

	virtual void render(ID3D11DeviceContext* context) override;


};

