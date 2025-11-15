#include "SelectMenuCoursesFolderItem.hpp"
#include "Common/FsUtils.hpp"

namespace
{
	constexpr StringView kDisplayName = U"Courses";
}

SelectMenuCoursesFolderItem::SelectMenuCoursesFolderItem(IsCurrentFolderYN isCurrentFolder)
	: m_isCurrentFolder(isCurrentFolder)
	, m_fullPath(kCoursesFolderSpecialPath)
{
}

void SelectMenuCoursesFolderItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_isCurrentFolder)
	{
		context.fnCloseFolder();
	}
	else
	{
		context.fnOpenCoursesFolder();
	}
}

void SelectMenuCoursesFolderItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isSong", false },
		{ U"center_isDirectory", true },
		{ U"center_isSubDirectory", false },
		{ U"center_isCourse", false },
		{ U"center_title", FolderDisplayNameCenter(kDisplayName, m_isCurrentFolder) },
	});
}

void SelectMenuCoursesFolderItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"isDirectory", true },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isCourse", false },
		{ paramNamePrefix + U"title", FolderDisplayNameTopBottom(kDisplayName, m_isCurrentFolder) },
	});
}

void SelectMenuCoursesFolderItem::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	// coursesディレクトリをエクスプローラで開く
	const FilePath coursesPath = FsUtils::CoursesDirectoryPath();
	if (FileSystem::IsDirectory(coursesPath))
	{
		System::ShowInFileManager(coursesPath);
	}
}
