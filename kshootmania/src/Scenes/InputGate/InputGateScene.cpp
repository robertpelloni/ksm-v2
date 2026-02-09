#include "InputGateScene.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Input/KeyConfig.hpp"

namespace
{
	// TODO: Create dedicated NocoUI layout for Input Gate
	// For now, we might use a placeholder or reuse a generic background
}

InputGateScene::InputGateScene()
{
	// Placeholder: Load a simple canvas or just display text
	// m_canvas = noco::Canvas::LoadFromFile(U"ui/scene/input_gate.noco");
}

Co::Task<void> InputGateScene::start()
{
	// Simple placeholder loop
	while (true)
	{
		co_await Co::NextFrame();

		update();

		if (KeyConfig::Down(kButtonBack) || KeyConfig::Down(kButtonStart))
		{
			break;
		}
	}

	requestNextScene<TitleScene>(TitleMenuItem::kInputGate);
}

void InputGateScene::update()
{
	if (m_canvas)
	{
		m_canvas->update();
	}
}

void InputGateScene::draw() const
{
	Scene::SetBackground(Palette::Black);

	if (m_canvas)
	{
		m_canvas->draw();
	}
	else
	{
		FontAsset(U"SystemFont")(U"INPUT GATE - Under Construction").drawAt(Scene::Center(), Palette::White);
		FontAsset(U"SystemFont")(U"Press Start or Back to Return").drawAt(Scene::Center().movedBy(0, 50), Palette::Gray);
	}
}
