#include "select_menu.hpp"
#include <cassert>
#include "kson/kson.hpp"
#include "menu_item/select_menu_song_item.hpp"
#include "menu_item/select_menu_all_folder_item.hpp"
#include "menu_item/select_menu_dir_folder_item.hpp"
#include "menu_item/select_menu_sub_dir_section_item.hpp"
#include "common/fs_utils.hpp"

namespace
{
	Array<FilePath> GetSubDirectories(FilePathView path)
	{
		Array<FilePath> directories =
			FileSystem::DirectoryContents(path, Recursive::No)
				.filter(
					[](FilePathView p)
					{
						return FileSystem::IsDirectory(p);
					});

		// フォルダ名(小文字変換)の昇順でソート
		directories.sort_by([](const FilePath& a, const FilePath& b)
		{
			return FsUtils::DirectoryNameByDirectoryPath(a).lowercased() < FsUtils::DirectoryNameByDirectoryPath(b).lowercased();
		});

		return directories;
	}
}

bool SelectMenu::openDirectory(FilePathView directoryPath, PlaySeYN playSe, RefreshSongPreviewYN refreshSongPreview, SaveToConfigIniYN saveToConfigIni)
{
	using Unicode::FromUTF8;

	if (playSe)
	{
		m_folderSelectSe.play();
	}

	// 曲の項目を挿入する関数
	// (挿入されたかどうかを返す)
	const auto fnInsertSongItem = [this](const String& songDirectory) -> bool
	{
		if (!FileSystem::IsDirectory(songDirectory))
		{
			return false;
		}

		std::unique_ptr<SelectMenuSongItem> item = std::make_unique<SelectMenuSongItem>(songDirectory);
		if (item->chartExists())
		{
			m_menu.push_back(std::move(item));
			return true;
		}
		else
		{
			return false;
		}
	};

	if (!directoryPath.empty())
	{
		if (!FileSystem::IsDirectory(directoryPath))
		{
			// ディレクトリが存在しない場合は何もしない
			return false;
		}

		m_menu.clear();
		m_jacketTextureCache.clear();
		m_iconTextureCache.clear();

		// ディレクトリの見出し項目を追加
		m_menu.push_back(std::make_unique<SelectMenuDirFolderItem>(IsCurrentFolderYN::Yes, FileSystem::FullPath(directoryPath)));

		// TODO: Insert course items

		// 曲の項目を追加
		Array<FilePath> subDirCandidates;
		{
			const Array<FilePath> songDirectories = GetSubDirectories(directoryPath);
			for (const auto& songDirectory : songDirectories)
			{
				// 項目追加
				const bool chartExists = fnInsertSongItem(songDirectory);

				// フォルダ直下に譜面がなかった場合はサブディレクトリの候補に追加
				if (!chartExists)
				{
					subDirCandidates.push_back(songDirectory);
				}
			}
		}

		// サブディレクトリ内の曲の項目を追加
		for (const auto& subDirCandidate : subDirCandidates)
		{
			// サブディレクトリの見出し項目を追加
			m_menu.push_back(std::make_unique<SelectMenuSubDirSectionItem>(FileSystem::FullPath(subDirCandidate))); // TODO: foldername.csvから読み込んだフォルダ名で置換

			bool chartExists = false;
			const Array<FilePath> songDirectories = GetSubDirectories(subDirCandidate);
			for (const auto& songDirectory : songDirectories)
			{
				// 項目追加
				if (fnInsertSongItem(songDirectory))
				{
					chartExists = true;
				}
			}

			// サブディレクトリ内に譜面が存在しなかった場合は見出し項目を削除
			if (!chartExists)
			{
				m_menu.pop_back();
			}
		}

		m_folderState.folderType = SelectFolderState::kDirectory;
		m_folderState.fullPath = FileSystem::FullPath(directoryPath);
	}
	else
	{
		m_menu.clear();
		m_jacketTextureCache.clear();
		m_iconTextureCache.clear();

		m_folderState.folderType = SelectFolderState::kNone;
		m_folderState.fullPath = U"";
	}

	// フォルダ項目を追加
	// (フォルダを開いていない場合、または現在開いていないフォルダを表示する設定の場合のみ)
	if (directoryPath.empty() || ConfigIni::GetBool(ConfigIni::Key::kAlwaysShowOtherFolders))
	{
		const Array<FilePath> searchPaths = {
			U"songs", // TODO: 設定可能にする
		};

		Array<FilePath> directories;
		for (const auto& path : searchPaths)
		{
			directories.append(GetSubDirectories(path).map([](FilePathView p) { return FileSystem::FullPath(p); }));
		}

		// フォルダ名(小文字変換)の昇順でソート
		directories.sort_by([](const FilePath& a, const FilePath& b)
		{
			return FsUtils::DirectoryNameByDirectoryPath(a).lowercased() < FsUtils::DirectoryNameByDirectoryPath(b).lowercased();
		});

		// フォルダを開いている場合は、そのフォルダが先頭になるような順番で項目を追加する必要があるので、現在開いているフォルダのインデックスを調べる
		std::size_t currentDirectoryIdx = 0;
		bool found = false;
		if (!directoryPath.empty())
		{
			const auto itr = std::find(directories.begin(), directories.end(), m_folderState.fullPath);
			if (itr != directories.end())
			{
				currentDirectoryIdx = static_cast<std::size_t>(std::distance(directories.begin(), itr));
				found = true;
			}
		}

		// 項目を追加
		for (std::size_t i = 0; i < directories.size(); ++i)
		{
			const std::size_t rotatedIdx = (i + currentDirectoryIdx) % directories.size();

			if (found && i == 0)
			{
				// 現在開いているフォルダは項目タイプkCurrentFolderとして既に追加済みなのでスキップ
				continue;
			}

			const auto& directory = directories[rotatedIdx];
			m_menu.push_back(std::make_unique<SelectMenuDirFolderItem>(IsCurrentFolderYN::No, FileSystem::FullPath(directory)));

			if (rotatedIdx == directories.size() - 1)
			{
				// "All"フォルダの項目を追加
				//m_menu.push_back(std::make_unique<SelectMenuAllFolderItem>(IsCurrentFolderYN::No));

				// TODO: "Courses"フォルダの項目を追加
			}
		}
	}

	if (saveToConfigIni)
	{
		ConfigIni::SetString(ConfigIni::Key::kSelectDirectory, directoryPath);
		ConfigIni::SetInt(ConfigIni::Key::kSelectSongIndex, 0);
	}

	refreshContentCanvasParams();

	if (refreshSongPreview)
	{
		this->refreshSongPreview();
	}

	return true;
}

void SelectMenu::setCursorAndSave(int32 cursor)
{
	m_menu.setCursor(cursor);
	ConfigIni::SetInt(ConfigIni::Key::kSelectSongIndex, cursor);
}

void SelectMenu::setCursorToItemByFullPath(FilePathView fullPath)
{
	// 大した数ではないので線形探索
	for (std::size_t i = 0U; i < m_menu.size(); ++i)
	{
		if (fullPath == m_menu[i]->fullPath())
		{
			setCursorAndSave(static_cast<int32>(i));
			break;
		}
	}
}

void SelectMenu::refreshContentCanvasParams()
{
	if (m_menu.empty())
	{
		return;
	}

	const int32 difficultyCursor = m_difficultyMenu.cursor(); // この値は-1にもなり得る
	const int32 difficultyIdx = difficultyCursor >= 0 ? difficultyCursor : m_difficultyMenu.rawCursor();
	m_selectSceneCanvas->setParamValues({
		{ U"difficultyCursorState", U"difficulty{}"_fmt(difficultyIdx) },
	});

	// 中央の項目のパラメータを反映
	if (const auto pItem = m_menu.cursorValue().get())
	{
		pItem->setCanvasParamsCenter(m_eventContext, *m_selectSceneCanvas, difficultyIdx);
	}

	// 上の項目のパラメータを反映
	for (int32 i = 0; i < kNumTopItems; ++i)
	{
		if (const auto pItem = m_menu.atCyclic(m_menu.cursor() - kNumTopItems + i).get())
		{
			const int32 topIdx = kNumTopItems - i;
			pItem->setCanvasParamsTopBottom(m_eventContext, *m_selectSceneCanvas, difficultyIdx, U"top{}_"_fmt(topIdx), U"TopItem{}"_fmt(topIdx));
		}
	}

	// 下の項目のパラメータを反映
	for (int32 i = 0; i < kNumBottomItems; ++i)
	{
		if (const auto pItem = m_menu.atCyclic(m_menu.cursor() + 1 + i).get())
		{
			const int32 bottomIdx = i + 1;
			pItem->setCanvasParamsTopBottom(m_eventContext, *m_selectSceneCanvas, difficultyIdx, U"bottom{}_"_fmt(bottomIdx), U"BottomItem{}"_fmt(bottomIdx));
		}
	}
}

void SelectMenu::refreshSongPreview()
{
	if (m_menu.empty() || m_menu.cursorValue() == nullptr)
	{
		m_songPreview.requestDefaultBgm();
		return;
	}

	const auto pChartInfo = m_menu.cursorValue()->chartInfoPtr(m_difficultyMenu.cursor());
	if (pChartInfo == nullptr)
	{
		m_songPreview.requestDefaultBgm();
	}
	else
	{
		m_songPreview.requestSongPreview(pChartInfo->previewBGMFilePath(), pChartInfo->previewBGMOffset(), pChartInfo->previewBGMDuration(), pChartInfo->previewBGMVolume());
	}
}

void SelectMenu::playShakeUpTween()
{
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeDown", false);

	// 再度再生できるようにオフにしてからオンにする
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeUp", false);
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeUp", true);
}

void SelectMenu::playShakeDownTween()
{
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeUp", false);

	// 再度再生できるようにオフにしてからオンにする
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeDown", false);
	m_selectSceneCanvas->setTweenActiveByTag(U"shakeDown", true);
}

SelectMenu::SelectMenu(const std::shared_ptr<noco::Canvas>& selectSceneCanvas, std::function<void(FilePathView, MusicGame::IsAutoPlayYN)> fnMoveToPlayScene)
	: m_selectSceneCanvas(selectSceneCanvas)
	, m_eventContext
		{
			.fnMoveToPlayScene = [fnMoveToPlayScene](FilePath path, MusicGame::IsAutoPlayYN isAutoPlay) { fnMoveToPlayScene(path, isAutoPlay); },
			.fnOpenDirectory = [this](FilePath path) { openDirectory(path, PlaySeYN::Yes); },
			.fnCloseFolder = [this]() { closeFolder(PlaySeYN::Yes); },
			.fnGetJacketTexture = [this](FilePathView path) -> const Texture& { return getJacketTexture(path); },
			.fnGetIconTexture = [this](FilePathView path) -> const Texture& { return getIconTexture(path); },
		}
	, m_menu(
		LinearMenu::CreateInfoWithCursorMinMax{
			.cursorInputCreateInfo = {
				.type = CursorInput::Type::Vertical,
				.buttonFlags = CursorButtonFlags::kArrowOrLaser,
				.buttonIntervalSec = 0.05,
				.buttonIntervalSecFirst = 0.3,
			},
			.cyclic = IsCyclicMenuYN::Yes,
		})
	, m_difficultyMenu(this)
{
	// ConfigIniのカーソルの値はopenDirectory内で上書きされるので事前に取得しておく
	const int32 loadedCursor = ConfigIni::GetInt(ConfigIni::Key::kSelectSongIndex);
	const int32 loadedDifficultyIdx = ConfigIni::GetInt(ConfigIni::Key::kSelectDifficulty);

	// 前回開いていたフォルダを復元
	if (openDirectory(ConfigIni::GetString(ConfigIni::Key::kSelectDirectory), PlaySeYN::No))
	{
		// 前回選択していたインデックスを復元
		m_menu.setCursor(loadedCursor);
		ConfigIni::SetInt(ConfigIni::Key::kSelectSongIndex, loadedCursor);

		// 前回選択していた難易度を復元
		m_difficultyMenu.setCursor(loadedDifficultyIdx);
		ConfigIni::SetInt(ConfigIni::Key::kSelectDifficulty, loadedDifficultyIdx);
	}
	else
	{
		openDirectory(U"", PlaySeYN::No);
	}

	refreshContentCanvasParams();
	refreshSongPreview();
}

SelectMenu::~SelectMenu() = default;

void SelectMenu::update()
{
	m_menu.update();
	if (const int32 deltaCursor = m_menu.deltaCursor(); deltaCursor != 0)
	{
		ConfigIni::SetInt(ConfigIni::Key::kSelectSongIndex, m_menu.cursor());
		m_songSelectSe.play();
		if (deltaCursor > 0)
		{
			playShakeDownTween();
		}
		else
		{
			playShakeUpTween();
		}
		refreshContentCanvasParams();
		refreshSongPreview();
	}

	// BT-B+C押下中は無視(プレイヤー切り替えと干渉しないようにするため)
	const bool btBCPressed = KeyConfig::Pressed(KeyConfig::kBT_B) && KeyConfig::Pressed(KeyConfig::kBT_C);
	if (!btBCPressed)
	{
		m_difficultyMenu.update();
		if (m_difficultyMenu.deltaCursor() != 0)
		{
			ConfigIni::SetInt(ConfigIni::Key::kSelectDifficulty, m_difficultyMenu.cursor());
			m_difficultySelectSe.play();
			refreshContentCanvasParams();
			refreshSongPreview();
		}
	}

	m_songPreview.update();
}

void SelectMenu::decide()
{
	if (m_menu.empty() || m_menu.cursorValue() == nullptr)
	{
		return;
	}

	m_menu.cursorValue()->decide(m_eventContext, m_difficultyMenu.cursor());
}

void SelectMenu::decideAutoPlay()
{
	if (m_menu.empty() || m_menu.cursorValue() == nullptr)
	{
		return;
	}

	m_menu.cursorValue()->decideAutoPlay(m_eventContext, m_difficultyMenu.cursor());
}

bool SelectMenu::isFolderOpen() const
{
	return m_folderState.folderType != SelectFolderState::kNone;
}

void SelectMenu::closeFolder(PlaySeYN playSe)
{
	if (playSe)
	{
		m_folderSelectSe.play();
	}

	// 元のパスを取得しておく
	const String originalFullPath = m_folderState.fullPath;

	// ルートディレクトリを開く
	openDirectory(U"", PlaySeYN::No);

	// カーソルを元々開いていたフォルダに合わせる
	setCursorToItemByFullPath(originalFullPath);

	ConfigIni::SetString(ConfigIni::Key::kSelectDirectory, U"");
	ConfigIni::SetInt(ConfigIni::Key::kSelectSongIndex, m_menu.cursor());

	refreshContentCanvasParams();
	refreshSongPreview();
}

const ISelectMenuItem& SelectMenu::cursorMenuItem() const
{
	return *m_menu.cursorValue();
}

bool SelectMenu::empty() const
{
	return m_menu.empty();
}

void SelectMenu::fadeOutSongPreviewForExit(Duration duration)
{
	m_songPreview.fadeOutForExit(duration);
}

void SelectMenu::reloadCurrentDirectory()
{
	const int32 currentCursor = m_menu.cursor();
	const int32 currentDifficulty = m_difficultyMenu.rawCursor();
	const FilePath currentDirectory = m_folderState.fullPath;

	openDirectory(currentDirectory, PlaySeYN::No, RefreshSongPreviewYN::No, SaveToConfigIniYN::No);

	m_menu.setCursor(currentCursor);
	m_difficultyMenu.setCursor(currentDifficulty);

	refreshContentCanvasParams();
}

const Texture& SelectMenu::getJacketTexture(FilePathView filePath)
{
	if (auto it = m_jacketTextureCache.find(filePath); it != m_jacketTextureCache.end())
	{
		return it->second;
	}

	Texture texture;
	if (FileSystem::IsFile(filePath))
	{
		texture = Texture{ filePath };
	}
	else
	{
		Logger << U"[ksm warning] SelectMenu::getJacketTexture: Jacket image file not found (filePath:'{}')"_fmt(filePath);
	}

	return m_jacketTextureCache.emplace(filePath, std::move(texture)).first->second;
}

const Texture& SelectMenu::getIconTexture(FilePathView filePath)
{
	if (auto it = m_iconTextureCache.find(filePath); it != m_iconTextureCache.end())
	{
		return it->second;
	}

	Texture texture;
	if (FileSystem::IsFile(filePath))
	{
		texture = Texture{ filePath };
	}
	else
	{
		Logger << U"[ksm warning] SelectMenu::getIconTexture: Icon image file not found (filePath:'{}')"_fmt(filePath);
	}

	return m_iconTextureCache.emplace(filePath, std::move(texture)).first->second;
}
