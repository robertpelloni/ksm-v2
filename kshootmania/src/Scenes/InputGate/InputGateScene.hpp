#pragma once
#include <CoTaskLib.hpp>
#include "InputGateClient.hpp"

class InputGateScene : public Co::SceneBase
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;
	InputGate::InputGateClient m_client;
	Array<InputGate::SongInfo> m_songList;

	// Download state
	bool m_isDownloading = false;
	double m_downloadProgress = 0.0;
	String m_downloadingTitle;

	int32 m_selectedIdx = 0;

	void populateSongList();

	void drawSongList() const;

	void drawDownloadProgress() const;

public:
	InputGateScene();

	virtual ~InputGateScene() = default;

	virtual Co::Task<void> start() override;

	virtual void update() override;

	virtual void draw() const override;
};
