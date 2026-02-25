#pragma once

namespace InputGate
{
	struct SongInfo
	{
		String id;
		String title;
		String artist;
		String jacketUrl;
		String downloadUrl;
		String previewUrl;
		String hash;
		int32 fileSize = 0;
	};

	struct UpdateInfo
	{
		bool hasUpdate = false;
		String currentVersion;
		String latestVersion;
		String downloadUrl;
		String patchNotes;
	};

	class InputGateClient
	{
	public:
		// 楽曲リストを取得する
		// モックまたはAPIコール
		Co::Task<Array<SongInfo>> fetchSongList();

		// 楽曲をダウンロードする
		// @param url ダウンロードURL
		// @param savePath 保存先パス
		// @param progressCallback 進捗コールバック (0.0 - 1.0)
		Co::Task<bool> downloadSong(StringView url, FilePathView savePath, std::function<void(double)> progressCallback = nullptr);

		// アップデートを確認する
		Co::Task<UpdateInfo> checkUpdate();
	};
}
