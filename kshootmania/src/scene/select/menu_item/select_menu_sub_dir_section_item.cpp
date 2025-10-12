#include "select_menu_sub_dir_section_item.hpp"
#include "common/fs_utils.hpp"

SelectMenuSubDirSectionItem::SelectMenuSubDirSectionItem(FilePathView fullPath)
	: m_fullPath(fullPath)
	, m_displayName(FsUtils::DirectoryNameByDirectoryPath(m_fullPath))
{
}

void SelectMenuSubDirSectionItem::decide([[maybe_unused]] const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	//Print << U"Not Implemented (SelectMenuSubDirSectionItem::decide)";
}

void SelectMenuSubDirSectionItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isDirectory", true },
		{ U"center_isLevel", false },
		{ U"center_isSong", false },
		{ U"center_title", m_displayName },
	});
}

void SelectMenuSubDirSectionItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", true },
		{ paramNamePrefix + U"isLevel", false },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", m_displayName },
	});
}
