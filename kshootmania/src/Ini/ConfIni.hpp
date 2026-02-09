#pragma once

namespace ConfIni
{
	namespace Key
	{
		constexpr StringView kFolder = U"folder";
	}

	/// @brief conf.iniを読み込み、追加の楽曲フォルダパスのリストを取得する
	/// @return 追加の楽曲フォルダパスのリスト
	[[nodiscard]]
	Array<FilePath> LoadAdditionalSongFolders();
}
