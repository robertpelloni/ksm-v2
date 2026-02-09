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
	// Fetch song list
	m_songList = co_await m_client.fetchSongList();
	populateSongList();

	// Updater for canvas
	const auto updateRunner = Co::UpdaterTask([this] { update(); }).runScoped();

	while (true)
	{
		co_await Co::NextFrame();

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
				// Since we are in a coroutine, we can await it?
				// But we want to show progress in update/draw loop.
				// So we spawn a task or just do it here if it supports progress callback?
				// InputGateClient::downloadSong is a Co::Task.
				// If we await it, the loop blocks, so update/draw won't run unless we run it in parallel?
				// But Co::Task is cooperative. If downloadSong awaits Co::NextFrame, it yields.
				// So we can await it here, BUT we need to ensure update/draw are called.
				// Currently start() runs, and it calls update(). draw() is called by the system (SceneManager).
				// So if we await here, draw() continues to be called?
				// Yes, Co::SceneBase::start() is a task. The SceneManager calls draw() every frame.
				// So awaiting here is fine, provided downloadSong yields periodically.

				const bool success = co_await m_client.downloadSong(song.downloadUrl, U"songs/download/{}.zip"_fmt(song.id), [this](double p) {
					m_downloadProgress = p;
				});

				m_isDownloading = false;

				if (success)
				{
					// Show success message or refresh?
					// For now just done.
				}
				else
				{
					// Show error?
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
