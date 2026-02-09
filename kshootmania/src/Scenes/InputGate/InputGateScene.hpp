#pragma once
#include <CoTaskLib.hpp>

class InputGateScene : public Co::SceneBase
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;

public:
	InputGateScene();

	virtual ~InputGateScene() = default;

	virtual Co::Task<void> start() override;

	virtual void update() override;

	virtual void draw() const override;
};
