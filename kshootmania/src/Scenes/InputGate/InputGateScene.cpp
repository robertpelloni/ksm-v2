#include "InputGateScene.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Input/KeyConfig.hpp"
#include "Common/FsUtils.hpp"

InputGateScene::InputGateScene()
{
	const FilePath uiFilePath = FsUtils::GetResourcePath(U"ui/scene/input_gate.noco");
	m_canvas = noco::Canvas::LoadFromFile(uiFilePath);
	if (!m_canvas)
	{
		// Fallback if file not found (though I just wrote it, resource path might differ in env)
		// For now, assume it works or just handle gracefully
	}
}

Co::Task<void> InputGateScene::start()
{
	while (true)
	{
		co_await Co::NextFrame();

		update();

		if (KeyConfig::Down(kButtonBack))
		{
			break;
		}

		if (KeyConfig::Down(kButtonStart))
		{
			// Simulate download
			System::MessageBoxOK(U"Download simulation started.", MessageBoxStyle::Info);
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
	if (m_canvas)
	{
		m_canvas->draw();
	}
	else
	{
		Scene::SetBackground(Palette::Black);
		FontAsset(U"SystemFont")(U"INPUT GATE (UI Load Failed)").drawAt(Scene::Center(), Palette::White);
	}
}
