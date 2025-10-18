#include "select_scene.hpp"
#include "scene/play_prepare/play_prepare_scene.hpp"
#include "scene/title/title_scene.hpp"
#include "ini/config_ini.hpp"
#include "common/fs_utils.hpp"

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

	Array<String> GetPlayerNames()
	{
		Array<String> playerNames;

		for (const auto& dirPath : FileSystem::DirectoryContents(U"score/", Recursive::No))
		{
			if (FileSystem::IsDirectory(dirPath))
			{
				playerNames.push_back(FsUtils::DirectoryNameByDirectoryPath(dirPath));
			}
		}

		if (playerNames.empty())
		{
			playerNames.push_back(U"PLAYER");
		}

		playerNames.sort_by([](const String& a, const String& b)
		{
			return a.lowercased() < b.lowercased();
		});

		return playerNames;
	}
}

void SelectScene::moveToPlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
{
	m_fadeOutColor = Palette::White;
	requestNextScene<PlayPrepareScene>(FilePath{ chartFilePath }, isAutoPlay);
}

void SelectScene::refreshCanvasPlayerName()
{
	const String playerName{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };
	m_canvas->setParamValue(U"playerName", playerName);
}

void SelectScene::updatePlayerSwitching()
{
	const bool btBCPressed = KeyConfig::Pressed(KeyConfig::kBT_B) && KeyConfig::Pressed(KeyConfig::kBT_C);
	if (!btBCPressed)
	{
		return;
	}

	const bool leftDown = KeyConfig::Down(KeyConfig::kLeft);
	const bool rightDown = KeyConfig::Down(KeyConfig::kRight);

	if (!leftDown && !rightDown)
	{
		return;
	}

	const String currentPlayer{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };

	auto it = std::find(m_playerNames.begin(), m_playerNames.end(), currentPlayer);
	int32 currentIndex = (it != m_playerNames.end()) ? static_cast<int32>(it - m_playerNames.begin()) : 0;

	if (leftDown)
	{
		currentIndex = (currentIndex - 1 + static_cast<int32>(m_playerNames.size())) % static_cast<int32>(m_playerNames.size());
	}
	else if (rightDown)
	{
		currentIndex = (currentIndex + 1) % static_cast<int32>(m_playerNames.size());
	}

	const String newPlayerName = m_playerNames[currentIndex];
	ConfigIni::SetString(ConfigIni::Key::kCurrentPlayer, newPlayerName);
	ConfigIni::Save();

	refreshCanvasPlayerName();
	m_menu.reloadCurrentDirectory();
}

SelectScene::SelectScene()
	: m_folderCloseButton(
		ConfigIni::GetInt(ConfigIni::Key::kSelectCloseFolderKey) == ConfigIni::Value::SelectCloseFolderKey::kBackButton
			? KeyConfig::kBack
			: KeyConfig::kBackspace)
	, m_canvas(LoadSelectSceneCanvas())
	, m_menu(m_canvas, [this](FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlayYN) { moveToPlayScene(chartFilePath, isAutoPlayYN); })
	, m_playerNames(GetPlayerNames())
{
	AutoMuteAddon::SetEnabled(true);

	// iniから読み込んだプレイヤー名が空文字列の場合は"PLAYER"に修正
	const String currentPlayer{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };
	if (currentPlayer.isEmpty())
	{
		ConfigIni::SetString(ConfigIni::Key::kCurrentPlayer, U"PLAYER");
	}

	refreshCanvasPlayerName();

	if (m_menu.empty())
	{
		System::MessageBoxOK(U"譜面データが見つかりませんでした。", MessageBoxStyle::Warning);
		m_skipFadeout = true;
		requestNextScene<TitleScene>(TitleMenuItem::kStart);
	}
}

void SelectScene::update()
{
	updatePlayerSwitching();

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
