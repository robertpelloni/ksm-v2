#pragma once
#include "select_folder_state.hpp"
#include "ui/array_with_linear_menu.hpp"
#include "select_difficulty_menu.hpp"
#include "select_song_preview.hpp"
#include "ksmaudio/ksmaudio.hpp"

using PlaySeYN = YesNo<struct PlaySeYN_tag>;

enum class SelectMenuShakeDirection
{
	kUnspecified,
	kUp,
	kDown,
};

class ISelectMenuItem;

struct SelectMenuEventContext
{
	// Note: FilePathViewやconst FilePath&ではなくFilePathにしているのは意図的
	//       (メニュー項目の再構築が発生すると呼び出し元のFilePathが無効になるので、事前にコピーしておく必要がある)
	std::function<void(FilePath, MusicGame::IsAutoPlayYN)> fnMoveToPlayScene;
	std::function<void(FilePath)> fnOpenDirectory;
	std::function<void()> fnCloseFolder;
	std::function<const Texture&(FilePathView)> fnGetJacketTexture;
	std::function<const Texture&(FilePathView)> fnGetIconTexture;
};

class SelectMenu
{
private:
	static constexpr int32 kNumDisplayItems = 8;
	static constexpr int32 kNumTopItems = kNumDisplayItems / 2;
	static constexpr int32 kNumBottomItems = kNumDisplayItems - kNumTopItems - 1;

	const SelectMenuEventContext m_eventContext;

	std::shared_ptr<noco::Canvas> m_selectSceneCanvas;

	SelectFolderState m_folderState;

	ArrayWithLinearMenu<std::unique_ptr<ISelectMenuItem>> m_menu;

	SelectDifficultyMenu m_difficultyMenu;

	SelectSongPreview m_songPreview;

	const ksmaudio::Sample m_songSelectSe{"se/sel_m.wav"};

	const ksmaudio::Sample m_difficultySelectSe{"se/sel_l.wav"};

	const ksmaudio::Sample m_folderSelectSe{"se/sel_dir.wav"};

	HashTable<String, Texture> m_jacketTextureCache;

	HashTable<String, Texture> m_iconTextureCache;

	bool openDirectory(FilePathView directoryPath, PlaySeYN playSe);

	void setCursorAndSave(int32 cursor);

	void setCursorToItemByFullPath(FilePathView fullPath);

	void refreshContentCanvasParams();

	void refreshSongPreview();

	void playShakeUpTween();

	void playShakeDownTween();

public:
	explicit SelectMenu(const std::shared_ptr<noco::Canvas>& selectSceneCanvas, std::function<void(FilePathView, MusicGame::IsAutoPlayYN)> fnMoveToPlayScene);

	~SelectMenu(); // ヘッダではISelectMenuItemが不完全型なのでソースファイル側で定義

	void update();

	void decide();

	void decideAutoPlay();

	bool isFolderOpen() const;

	void closeFolder(PlaySeYN playSe);

	const ISelectMenuItem& cursorMenuItem() const;

	bool empty() const;

	void fadeOutSongPreviewForExit(Duration duration);

	const Texture& getJacketTexture(FilePathView filePath);

	const Texture& getIconTexture(FilePathView filePath);
};
