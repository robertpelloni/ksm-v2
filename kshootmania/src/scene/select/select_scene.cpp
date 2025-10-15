#include "select_scene.hpp"
#include "scene/play_prepare/play_prepare_scene.hpp"
#include "scene/title/title_scene.hpp"

namespace
{
	constexpr Duration kFadeInDuration = 0.25s;
	constexpr Duration kFadeOutDuration = 0.4s;

	constexpr FilePathView kSelectSceneUIFilePath = U"ui/scene/select.noco";

	std::shared_ptr<noco::Canvas> LoadSelectSceneCanvas()
	{
		const auto canvas = noco::Canvas::LoadFromFile(kSelectSceneUIFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(kSelectSceneUIFilePath) };
		}
		return canvas;
	}
}

void SelectScene::moveToPlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
{
	m_fadeOutColor = Palette::White;
	requestNextScene<PlayPrepareScene>(FilePath{ chartFilePath }, isAutoPlay);
}

SelectScene::SelectScene()
	: m_folderCloseButton(
		ConfigIni::GetInt(ConfigIni::Key::kSelectCloseFolderKey) == ConfigIni::Value::SelectCloseFolderKey::kBackButton
			? KeyConfig::kBack
			: KeyConfig::kBackspace)
	, m_canvas(LoadSelectSceneCanvas())
	, m_menu(m_canvas, [this](FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlayYN) { moveToPlayScene(chartFilePath, isAutoPlayYN); })
{
	AutoMuteAddon::SetEnabled(true);

	if (m_menu.empty())
	{
		System::MessageBoxOK(U"譜面データが見つかりませんでした。", MessageBoxStyle::Warning);
		m_skipFadeout = true;
		requestNextScene<TitleScene>(TitleMenuItem::kStart);
	}
}

void SelectScene::update()
{
	const bool closeFolder = m_menu.isFolderOpen() && KeyConfig::Down(m_folderCloseButton/* ← kBackspace・kBackのいずれかが入っている */);

	// Backボタン(Escキー)を押した場合、(フォルダを閉じる状況でなければ)タイトル画面へ戻る
	if (!closeFolder && KeyConfig::Down(KeyConfig::kBack))
	{
		m_menu.fadeOutSongPreviewForExit(kFadeOutDuration);
		m_fadeOutColor = Palette::Black;
		requestNextScene<TitleScene>(TitleMenuItem::kStart);
		return;
	}

	m_menu.update();

	// BackSpaceキーまたはBackボタン(Escキー)でフォルダを閉じる
	if (closeFolder)
	{
		m_menu.closeFolder(PlaySeYN::No);
	}

	// スタートボタンを押した場合、フォルダを開く または プレイ開始
	// Shift+スタートボタンの場合はオートプレイ開始
	if (KeyConfig::Down(KeyConfig::kStart))
	{
		if (KeyShift.pressed())
		{
			m_menu.decideAutoPlay();
		}
		else
		{
			m_menu.decide();
		}
	}

	// オートプレイボタン(F11)を押した場合、オートプレイ開始
	if (KeyConfig::Down(KeyConfig::kAutoPlay))
	{
		m_menu.decideAutoPlay();
	}

	m_canvas->update();
}

void SelectScene::draw() const
{
	m_canvas->draw();
}

Co::Task<void> SelectScene::fadeIn()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeIn(kFadeInDuration);
}

Co::Task<void> SelectScene::fadeOut()
{
	if (m_skipFadeout)
	{
		co_return;
	}

	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeOut(kFadeOutDuration, m_fadeOutColor);
}
