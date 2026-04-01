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

	struct RankingEntry
	{
		int32 rank;
		String playerName;
		int32 score;
		String playStyle; // e.g., "Keyboard", "Gamepad"
		String date;
	};

	class InputGateClient
	{
	public:
		// 楽曲リストを取得する
		// モックまたはAPIコール
		Co::Task<Array<SongInfo>> fetchSongList();

		// ランキング情報を取得する
		// @param songId 対象の楽曲ID
		// @param difficulty 難易度(オプション)
		Co::Task<Array<RankingEntry>> fetchRanking(StringView songId, int32 difficulty = 0);

		// 楽曲をダウンロードする
		// @param url ダウンロードURL
		// @param savePath 保存先パス
		// @param progressCallback 進捗コールバック (0.0 - 1.0)
		Co::Task<int32> downloadSong(StringView url, FilePathView savePath, std::function<void(double)> progressCallback = nullptr);

		// アップデートを確認する
		Co::Task<UpdateInfo> checkUpdate();
	};
}
