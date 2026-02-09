#include "InputGateScene.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Input/KeyConfig.hpp"
#include "Common/FsUtils.hpp"
#include "NocoExtensions/NocoUtils.hpp"

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

void InputGateScene::populateSongList()
{
	if (!m_canvas) return;

	// ContentAreaの子要素としてボタンを追加する処理
	// Note: NocoUIの現在の仕様ではC++から動的にレイアウトを追加するのが難しいため、
	// ここではモックデータをログ出力するにとどめるか、あるいはあらかじめ定義されたスロットに値を埋める形にする。
	// 今回は簡易的に、テキストでリストを表示する形にする(NocoUI上にテキストを描画するのではなく、コンソールログ出力などで動作確認)

	// NocoUIのLabelを更新してリストを表示してみる(仮)
	// 本当はScrollViewなどが欲しいが、NocoUIの仕様依存。
	// ここでは"ContentArea"の中にテキストを表示するハックを行うか、Siv3Dの標準機能で描画を重ねる。
}

Co::Task<void> InputGateScene::start()
{
	// データ取得
	m_songList = co_await m_client.fetchSongList();
	populateSongList();

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
			if (!m_songList.empty())
			{
				const auto& song = m_songList[0];
				System::MessageBoxOK(U"Downloading: {}"_fmt(song.title), MessageBoxStyle::Info);
			}
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

		// 仮の実装: リストをSiv3Dの標準機能で描画
		// 将来的にはNocoUIのListウィジェットなどを使用する
		if (!m_songList.empty())
		{
			const Font& font = FontAsset(U"SystemFont");
			int32 y = 130;
			for (const auto& song : m_songList)
			{
				font(U"{} / {}"_fmt(song.title, song.artist)).draw(40, y, Palette::White);
				y += 40;
			}
		}
	}
	else
	{
		Scene::SetBackground(Palette::Black);
		FontAsset(U"SystemFont")(U"INPUT GATE (UI Load Failed)").drawAt(Scene::Center(), Palette::White);
	}
}
