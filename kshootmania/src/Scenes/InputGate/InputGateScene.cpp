#include "InputGateScene.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Input/KeyConfig.hpp"
#include "Common/FsUtils.hpp"
#include "Common/AssetManagement.hpp"
#include "NocoExtensions/NocoUtils.hpp"
#include "I18n/I18n.hpp"

namespace
{
	constexpr int32 kItemHeight = 40;
	constexpr int32 kListX = 40;
	constexpr int32 kListY = 130;
	constexpr int32 kListMaxItems = 10; // Number of items to show at once (scrolling)
}

InputGateScene::InputGateScene()
{
	const FilePath uiFilePath = FsUtils::GetResourcePath(U"ui/scene/input_gate.noco");
	m_canvas = noco::Canvas::LoadFromFile(uiFilePath);
	if (!m_canvas)
	{
		Logger << U"[ksm error] Failed to load input_gate.noco";
	}
}

void InputGateScene::populateSongList()
{
	// NocoUI is not dynamic enough yet, so we handle list drawing manually in drawSongList()
	// But we might want to update some static labels if they exist
}

Co::Task<void> InputGateScene::start()
{
	// Updater for canvas
	const auto updateRunner = Co::UpdaterTask([this] { update(); }).runScoped();

	// Check for updates first
	m_updateInfo = co_await m_client.checkUpdate();
	if (m_updateInfo.hasUpdate)
	{
		m_showUpdateDialog = true;
	}

	// Fetch song list
	m_songList = co_await m_client.fetchSongList();
	populateSongList();

	while (true)
	{
		co_await Co::NextFrame();

		if (m_showUpdateDialog)
		{
			// Modal dialog handling
			if (KeyConfig::Down(kButtonStart) || KeyConfig::Down(kButtonBack))
			{
				if (KeyConfig::Down(kButtonStart) && !m_updateInfo.downloadUrl.isEmpty())
				{
					// Open browser
					System::LaunchBrowser(m_updateInfo.downloadUrl);
				}
				m_showUpdateDialog = false;
			}
			continue;
		}

		if (m_isDownloading)
		{
			// Modal blocking input
			continue;
		}

		// Navigation
		if (KeyConfig::Down(kButtonBack))
		{
			break;
		}

		if (!m_songList.empty())
		{
			if (KeyConfig::Down(kButtonDown))
			{
				m_selectedIdx = (m_selectedIdx + 1) % static_cast<int32>(m_songList.size());
			}
			else if (KeyConfig::Down(kButtonUp))
			{
				m_selectedIdx = (m_selectedIdx - 1 + static_cast<int32>(m_songList.size())) % static_cast<int32>(m_songList.size());
			}

			if (KeyConfig::Down(kButtonStart))
			{
				const auto& song = m_songList[m_selectedIdx];
				m_isDownloading = true;
				m_downloadProgress = 0.0;
				m_downloadingTitle = song.title;

				// Start download
				const FilePath zipPath = U"songs/download/{}.zip"_fmt(song.id);

				// Ensure directory exists
				if (!FileSystem::Exists(U"songs/download/"))
				{
					FileSystem::CreateDirectories(U"songs/download/");
				}

				const bool success = co_await m_client.downloadSong(song.downloadUrl, zipPath, [this](double p) {
					m_downloadProgress = p;
				});

				m_isDownloading = false;

				if (success)
				{
					// Extract ZIP
					// Siv3D's ZIPReader
					const FilePath extractPath = U"songs/download/{}"_fmt(song.id);
					ZIPReader zip{ zipPath };
					if (zip)
					{
						zip.extractAll(extractPath);
						// Remove zip file? Or keep it?
						// FileSystem::Remove(zipPath);
					}
					else
					{
						Logger << U"[ksm error] Failed to open ZIP: " << zipPath;
					}
				}
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
		drawSongList();

		if (m_isDownloading)
		{
			drawDownloadProgress();
		}

		if (m_showUpdateDialog)
		{
			drawUpdateDialog();
		}
	}
	else
	{
		Scene::SetBackground(Palette::Black);
		AssetManagement::SystemFont()(U"INPUT GATE (UI Load Failed)").drawAt(Scene::Center(), Palette::White);
	}
}

void InputGateScene::drawSongList() const
{
	if (m_songList.empty()) return;

	const Font& drawFont = AssetManagement::SystemFont();

	// Calculate scroll window
	int32 startIdx = 0;
	if (m_selectedIdx >= kListMaxItems)
	{
		startIdx = m_selectedIdx - kListMaxItems + 1;
	}

	int32 y = kListY;
	for (int32 i = 0; i < kListMaxItems; ++i)
	{
		const int32 idx = startIdx + i;
		if (idx >= static_cast<int32>(m_songList.size())) break;

		const auto& song = m_songList[idx];
		const bool isSelected = (idx == m_selectedIdx);

		const RectF itemRect(kListX, y, 500, kItemHeight); // Fixed width for now

		if (isSelected)
		{
			itemRect.draw(ColorF(0.3, 0.3, 0.8, 0.5));
			itemRect.drawFrame(2, Palette::Cyan);
		}
		else
		{
			itemRect.draw(ColorF(0.2, 0.2, 0.2, 0.5));
		}

		drawFont(U"{} / {}"_fmt(song.title, song.artist)).drawAt(itemRect.center(), Palette::White);

		y += kItemHeight + 5;
	}
}

void InputGateScene::drawDownloadProgress() const
{
	// Overlay background
	Scene::Rect().draw(ColorF(0.0, 0.7));

	const RectF box(Arg::center = Scene::Center(), 400, 150);
	box.draw(Palette::Black).drawFrame(2, Palette::White);

	const Font& font = AssetManagement::SystemFont();

	// Title
	font(I18n::Get(I18n::InputGate::kDownloadingChart) + U"\n" + m_downloadingTitle)
		.drawAt(box.center().movedBy(0, -30), Palette::White);

	// Progress bar
	const RectF barBg(Arg::center = box.center().movedBy(0, 30), 300, 20);
	barBg.draw(Palette::Gray);

	const double w = barBg.w * Clamp(m_downloadProgress, 0.0, 1.0);
	RectF(barBg.pos, w, barBg.h).draw(Palette::Cyan);

	// Percentage
	font(U"{:.0f}%"_fmt(m_downloadProgress * 100))
		.drawAt(barBg.center(), Palette::Black);
}

void InputGateScene::drawUpdateDialog() const
{
	// Overlay
	Scene::Rect().draw(ColorF(0.0, 0.8));

	const RectF dialog(Arg::center = Scene::Center(), 600, 400);
	dialog.draw(Palette::Black).drawFrame(2, Palette::Yellow);

	const Font& font = AssetManagement::SystemFont();
	const Font& titleFont = AssetManagement::SystemFont(); // Use a larger font if available

	// Title
	titleFont(U"New Update Available!").drawAt(dialog.topCenter().movedBy(0, 40), Palette::Yellow);

	// Versions
	font(U"Current: {}"_fmt(m_updateInfo.currentVersion)).drawAt(dialog.topCenter().movedBy(0, 80), Palette::White);
	font(U"Latest: {}"_fmt(m_updateInfo.latestVersion)).drawAt(dialog.topCenter().movedBy(0, 110), Palette::Cyan);

	// Patch Notes
	const RectF notesRect = dialog.stretched(-20, -140, -20, -80);
	notesRect.draw(ColorF(0.2));
	font(m_updateInfo.patchNotes).draw(notesRect.stretched(-10), Palette::White);

	// Buttons
	font(U"[Start] Download / Open Browser").drawAt(dialog.bottomCenter().movedBy(0, -50), Palette::White);
	font(U"[Back] Close").drawAt(dialog.bottomCenter().movedBy(0, -20), Palette::Gray);
}
