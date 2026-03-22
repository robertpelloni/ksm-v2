#pragma once
#include <CoTaskLib.hpp>
#include "InputGateClient.hpp"

class InputGateScene : public Co::SceneBase
{
private:
	enum class TabState
	{
		Songs,
		Ranking
	};

	std::shared_ptr<noco::Canvas> m_canvas;
	InputGate::InputGateClient m_client;

	TabState m_currentTab = TabState::Songs;

	// Songs state
	Array<InputGate::SongInfo> m_songList;
	int32 m_selectedSongIdx = 0;

	// Ranking state
	Array<InputGate::RankingEntry> m_rankingList;
	bool m_isFetchingRanking = false;
	int32 m_selectedRankingDiff = 0;

	// Download state
	bool m_isDownloading = false;
	double m_downloadProgress = 0.0;
	String m_downloadingTitle;

	// Update state
	InputGate::UpdateInfo m_updateInfo;
	bool m_showUpdateDialog = false;

	void populateSongList();
	void fetchRankingsForCurrentSong();

	void drawSongList() const;
	void drawRankingList() const;
	void drawTabs() const;

	void drawDownloadProgress() const;
	void drawUpdateDialog() const;

public:
	InputGateScene();

	virtual ~InputGateScene() = default;

	virtual Co::Task<void> start() override;

	virtual void update() override;

	virtual void draw() const override;
};
