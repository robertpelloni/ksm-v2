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

	constexpr int32 kTabX = 40;
	constexpr int32 kTabY = 80;
	constexpr int32 kTabWidth = 150;
	constexpr int32 kTabHeight = 40;
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

void InputGateScene::fetchRankingsForCurrentSong()
{
	if (m_songList.empty() || m_isFetchingRanking) return;

	const auto& song = m_songList[m_selectedSongIdx];
	m_isFetchingRanking = true;

	// Fire and forget task to fetch rankings, updating state when done
	// We can't await inside a non-coroutine, so we spawn an anonymous task or use a coroutine helper.
	// Actually, wait, we are in `start()` which is a Co::Task. We can await there.
	// But we need to switch tabs via input, which is handled in `start()`.
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

		// Tab Switching (FX-L / FX-R)
		if (KeyConfig::Down(kButtonFX_L) || KeyConfig::Down(kButtonFX_R))
		{
			if (m_currentTab == TabState::Songs)
			{
				m_currentTab = TabState::Ranking;

				// Fetch rankings when switching to ranking tab
				if (!m_songList.empty())
				{
					m_isFetchingRanking = true;
					const auto& song = m_songList[m_selectedSongIdx];
					m_rankingList = co_await m_client.fetchRanking(song.id, m_selectedRankingDiff);
					m_isFetchingRanking = false;
				}
			}
			else
			{
				m_currentTab = TabState::Songs;
			}
		}

		if (m_currentTab == TabState::Songs)
		{
			if (!m_songList.empty())
			{
				if (KeyConfig::Down(kButtonDown))
				{
					m_selectedSongIdx = (m_selectedSongIdx + 1) % static_cast<int32>(m_songList.size());
				}
				else if (KeyConfig::Down(kButtonUp))
				{
					m_selectedSongIdx = (m_selectedSongIdx - 1 + static_cast<int32>(m_songList.size())) % static_cast<int32>(m_songList.size());
				}

				if (KeyConfig::Down(kButtonStart))
				{
					if (!m_downloadErrorMsg.isEmpty())
					{
						// Dismiss error
						m_downloadErrorMsg.clear();
					}
					else
					{
						const auto& song = m_songList[m_selectedSongIdx];
						m_isDownloading = true;
						m_downloadProgress = 0.0;
						m_downloadingTitle = song.title;
						m_downloadErrorMsg.clear();

						// Start download
						const FilePath zipPath = U"songs/download/{}.zip"_fmt(song.id);

						// Ensure directory exists
						if (!FileSystem::Exists(U"songs/download/"))
						{
							FileSystem::CreateDirectories(U"songs/download/");
						}

						const int32 result = co_await m_client.downloadSong(song.downloadUrl, zipPath, [this](double p) {
							m_downloadProgress = p;
						});

						m_isDownloading = false;

						if (result == static_cast<int32>(InputGate::DownloadResult::Success))
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
								m_downloadErrorMsg = U"Failed to open ZIP:\n" + zipPath;
								Logger << U"[ksm error] Failed to open ZIP: " << zipPath;
							}
						}
						else if (result == static_cast<int32>(InputGate::DownloadResult::NetworkError))
						{
							m_downloadErrorMsg = U"Network Error while downloading.\nPlease check your connection.";
						}
						else if (result == static_cast<int32>(InputGate::DownloadResult::FileWriteError))
						{
							m_downloadErrorMsg = U"File Write Error.\nPlease check disk space or permissions.";
						}
					}
				}
			}
		}
		else if (m_currentTab == TabState::Ranking)
		{
			// Ranking tab logic (maybe difficulty switching)
			if (KeyConfig::Down(kButtonRight))
			{
				m_selectedRankingDiff = (m_selectedRankingDiff + 1) % 4; // Assuming 4 diffs max

				if (!m_songList.empty())
				{
					m_isFetchingRanking = true;
					const auto& song = m_songList[m_selectedSongIdx];
					m_rankingList = co_await m_client.fetchRanking(song.id, m_selectedRankingDiff);
					m_isFetchingRanking = false;
				}
			}
			else if (KeyConfig::Down(kButtonLeft))
			{
				m_selectedRankingDiff = (m_selectedRankingDiff - 1 + 4) % 4;

				if (!m_songList.empty())
				{
					m_isFetchingRanking = true;
					const auto& song = m_songList[m_selectedSongIdx];
					m_rankingList = co_await m_client.fetchRanking(song.id, m_selectedRankingDiff);
					m_isFetchingRanking = false;
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

		drawTabs();

		if (m_currentTab == TabState::Songs)
		{
			drawSongList();
		}
		else if (m_currentTab == TabState::Ranking)
		{
			drawRankingList();
		}

		if (m_isDownloading)
		{
			drawDownloadProgress();
		}

		if (!m_downloadErrorMsg.isEmpty())
		{
			drawErrorDialog();
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

void InputGateScene::drawTabs() const
{
	const Font& font = AssetManagement::SystemFont();

	// Songs Tab
	RectF songsTab(kTabX, kTabY, kTabWidth, kTabHeight);
	if (m_currentTab == TabState::Songs)
	{
		songsTab.draw(Palette::Cyan);
		font(U"Song List").drawAt(songsTab.center(), Palette::Black);
	}
	else
	{
		songsTab.draw(Palette::Gray);
		font(U"Song List").drawAt(songsTab.center(), Palette::White);
	}

	// Ranking Tab
	RectF rankingTab(kTabX + kTabWidth + 10, kTabY, kTabWidth, kTabHeight);
	if (m_currentTab == TabState::Ranking)
	{
		rankingTab.draw(Palette::Cyan);
		font(U"Ranking").drawAt(rankingTab.center(), Palette::Black);
	}
	else
	{
		rankingTab.draw(Palette::Gray);
		font(U"Ranking").drawAt(rankingTab.center(), Palette::White);
	}

	font(U"[FX-L/FX-R] Switch Tab").drawAt(kTabX + kTabWidth * 2 + 100, kTabY + kTabHeight / 2, Palette::Yellow);
}

void InputGateScene::drawSongList() const
{
	if (m_songList.empty()) return;

	const Font& drawFont = AssetManagement::SystemFont();

	// Calculate scroll window
	int32 startIdx = 0;
	if (m_selectedSongIdx >= kListMaxItems)
	{
		startIdx = m_selectedSongIdx - kListMaxItems + 1;
	}

	int32 y = kListY;
	for (int32 i = 0; i < kListMaxItems; ++i)
	{
		const int32 idx = startIdx + i;
		if (idx >= static_cast<int32>(m_songList.size())) break;

		const auto& song = m_songList[idx];
		const bool isSelected = (idx == m_selectedSongIdx);

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

void InputGateScene::drawRankingList() const
{
	const Font& font = AssetManagement::SystemFont();

	if (m_songList.empty())
	{
		font(U"No song selected.").drawAt(Scene::Center(), Palette::White);
		return;
	}

	const auto& song = m_songList[m_selectedSongIdx];

	// Header
	font(U"Internet Ranking for: {}"_fmt(song.title)).draw(kListX, kListY, Palette::White);

	const String diffNames[] = { U"Light", U"Challenge", U"Extended", U"Infinite" };
	font(U"Difficulty: {} (< / > to change)"_fmt(diffNames[m_selectedRankingDiff])).draw(kListX, kListY + 30, Palette::Yellow);

	if (m_isFetchingRanking)
	{
		font(U"Fetching rankings...").drawAt(Scene::Center(), Palette::Cyan);
		return;
	}

	if (m_rankingList.empty())
	{
		font(U"No rankings available.").draw(kListX, kListY + 80, Palette::Gray);
		return;
	}

	int32 y = kListY + 80;
	for (const auto& entry : m_rankingList)
	{
		const RectF itemRect(kListX, y, 600, kItemHeight);
		itemRect.draw(ColorF(0.1, 0.5));
		itemRect.drawFrame(1, Palette::Gray);

		// Rank, Name, Score, Date
		font(U"#{}"_fmt(entry.rank)).draw(kListX + 10, y + 5, Palette::Yellow);
		font(entry.playerName).draw(kListX + 60, y + 5, Palette::White);
		font(U"{:07d}"_fmt(entry.score)).draw(kListX + 250, y + 5, Palette::Cyan);
		font(entry.date).draw(kListX + 400, y + 5, Palette::Gray);
		font(entry.playStyle).draw(kListX + 500, y + 5, Palette::White);

		y += kItemHeight + 5;

		if (y > Scene::Height() - 50) break; // Don't overflow screen
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

void InputGateScene::drawErrorDialog() const
{
	// Overlay
	Scene::Rect().draw(ColorF(0.0, 0.8));

	const RectF dialog(Arg::center = Scene::Center(), 500, 200);
	dialog.draw(Palette::Black).drawFrame(2, Palette::Red);

	const Font& font = AssetManagement::SystemFont();

	font(U"Download Failed").drawAt(dialog.topCenter().movedBy(0, 30), Palette::Red);
	font(m_downloadErrorMsg).drawAt(dialog.center(), Palette::White);
	font(U"[Start] OK").drawAt(dialog.bottomCenter().movedBy(0, -30), Palette::White);
}
