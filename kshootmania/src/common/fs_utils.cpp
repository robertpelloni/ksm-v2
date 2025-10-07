#include "fs_utils.hpp"

namespace FsUtils
{
	FilePath GetFullPathInFolder(SpecialFolder folder, FilePathView relativePath)
	{
		return FileSystem::PathAppend(FileSystem::GetFolderPath(folder), relativePath);
	}

	FilePath AppDirectoryPath()
	{
#ifdef __APPLE__
		FilePath modulePath = FileSystem::ModulePath();
		// macOSのAppBundleの場合、ModulePathが既に.appまでのパスを返す
		if (modulePath.ends_with(U".app"))
		{
			return modulePath;
		}
		else
		{
			return FileSystem::ParentPath(modulePath);
		}
#else
		return FileSystem::ParentPath(FileSystem::ModulePath());
#endif
	}

	String DirectoryNameByDirectoryPath(FilePathView directoryPath)
	{
		const bool endsWithSlash = directoryPath.ends_with(U'/') || directoryPath.ends_with(U'\\');
		return FileSystem::FileName(endsWithSlash ? directoryPath.substr(0, directoryPath.size() - 1) : directoryPath);
	}

	String EliminateExtension(FilePathView path)
	{
		if (path.empty())
		{
			return String{};
		}
		const auto extension = FileSystem::Extension(path);
		if (extension.empty())
		{
			return String{ path };
		}
		return String{ path.substr(0, path.size() - extension.size() - 1) }; // ドット除去のため-1
	}
}
