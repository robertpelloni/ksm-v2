#pragma once

namespace InputGate
{
	struct SongInfo
	{
		String title;
		String artist;
		String jacketUrl;
		String downloadUrl;
	};

	class InputGateClient
	{
	public:
		// 楽曲リストを取得する(モック)
		Co::Task<Array<SongInfo>> fetchSongList();
	};
}
