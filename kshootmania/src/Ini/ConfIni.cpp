#include "ConfIni.hpp"
#include "Common/FsUtils.hpp"
#include "Common/Encoding.hpp"

namespace ConfIni
{
	Array<FilePath> LoadAdditionalSongFolders()
	{
		Array<FilePath> folders;

		// songsフォルダ直下のconf.iniを探す
		// (v1の仕様ではsongs/conf.ini)
		const FilePath confIniPath = FileSystem::PathAppend(FsUtils::SongsDirectoryPath(), U"conf.ini");

		if (!FileSystem::IsFile(confIniPath))
		{
			return folders;
		}

		// ファイルを行ごとに読み込む
		const Array<String> lines = Encoding::ReadTextFileLinesShiftJISOrUTF8BasedOnBOM(confIniPath);

		for (const String& line : lines)
		{
			const String trimmedLine = line.trimmed();

			// コメントや空行をスキップ
			if (trimmedLine.isEmpty() || trimmedLine.starts_with(U';') || trimmedLine.starts_with(U'#'))
			{
				continue;
			}

			// folder=パス の形式をパース
			// (v1仕様に準拠し、キーは大文字小文字を区別しない想定だが、とりあえず単純なprefix matchで実装)
			if (trimmedLine.starts_with(Key::kFolder + U"="))
			{
				const String path = trimmedLine.substr(Key::kFolder.length() + 1).trimmed();
				if (!path.isEmpty())
				{
					// 相対パスの場合はsongsフォルダ基準にするか、実行ファイル基準にするか？
					// v1では絶対パス指定が主だったが、相対パスも許容すべき。
					// ここではFileSystem::FullPathで解決する(カレントディレクトリ基準=ResourceDirectoryPath基準になる可能性が高いが、
					// ユーザーが絶対パスを書くケースが多いと思われる)

					// パス末尾の\や/を除去
					FilePath normalizedPath = path;
					if (normalizedPath.ends_with(U'/') || normalizedPath.ends_with(U'\\'))
					{
						normalizedPath.pop_back();
					}

					if (FileSystem::IsDirectory(normalizedPath))
					{
						folders.push_back(FileSystem::FullPath(normalizedPath));
					}
				}
			}
		}

		return folders;
	}
}
