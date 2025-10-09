#pragma once
#include <CoTaskLib.hpp>
#include "select_menu.hpp"
#include "select_folder_state.hpp"

class SelectScene : public Co::UpdaterSceneBase
{
private:
	const KeyConfig::Button m_folderCloseButton;

	ColorF m_fadeOutColor = Palette::Black;

	bool m_skipFadeout = false;

	std::shared_ptr<noco::Canvas> m_canvas;

	SelectMenu m_menu;

	void moveToPlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay);

public:
	SelectScene();

	virtual void update() override;

	virtual void draw() const override;

	virtual Co::Task<void> fadeIn() override;

	virtual Co::Task<void> fadeOut() override;
};
