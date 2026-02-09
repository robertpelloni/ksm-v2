#include "InputGateClient.hpp"
#include <Siv3D.hpp>

namespace InputGate
{
	namespace
	{
		// モック用URL (実際はconfigなどで管理するのが望ましい)
		constexpr StringView kApiBaseUrl = U"http://localhost:8080/api/inputgate";
		constexpr bool kUseMock = true;
	}

	Co::Task<Array<SongInfo>> InputGateClient::fetchSongList()
	{
		if constexpr (kUseMock)
		{
			// ネットワーク遅延をシミュレート
			co_await Co::Delay(0.5s);

			// ダミーデータを返す
			co_return Array<SongInfo>{
				{ U"song_001", U"Test Song 1", U"Test Artist 1", U"http://example.com/jacket1.jpg", U"http://example.com/song1.zip", U"", U"hash1", 1024 * 1024 * 5 },
				{ U"song_002", U"Test Song 2", U"Test Artist 2", U"http://example.com/jacket2.jpg", U"http://example.com/song2.zip", U"", U"hash2", 1024 * 1024 * 3 },
				{ U"song_003", U"K-Shoot MANIA Theme", U"K-Shoot MANIA", U"", U"", U"", U"hash3", 1024 * 1024 * 2 },
				{ U"song_004", U"New Feature Demo", U"Dev Team", U"", U"", U"", U"hash4", 1024 * 1024 * 8 },
				{ U"song_005", U"Network Test", U"Server", U"", U"", U"", U"hash5", 1024 * 1024 * 1 },
			};
		}
		else
		{
			// TODO: 実装
			// const URL url = kApiBaseUrl + U"/list";
			// const auto response = SimpleHTTP::Get(url);
			// if (response.isOK()) { ... }
			co_await Co::Delay(0.1s);
			co_return Array<SongInfo>();
		}
	}

	Co::Task<bool> InputGateClient::downloadSong(StringView url, FilePathView savePath, std::function<void(double)> progressCallback)
	{
		if constexpr (kUseMock)
		{
			// モック: プログレスを進めながら完了させる
			double progress = 0.0;
			while (progress < 1.0)
			{
				progress += 0.05; // 20フレームで完了
				if (progressCallback)
				{
					progressCallback(progress);
				}
				co_await Co::NextFrame();
			}
			co_return true;
		}
		else
		{
			// TODO: SimpleHTTP::Save(url, savePath)
			// 残念ながらSiv3DのSimpleHTTP::Saveは非同期プログレスコールバックを標準で持っていない場合がある
			// HTTPClientなどの利用を検討するか、自前で実装が必要。
			// 今回は簡易的に成功を返す。
			co_return true;
		}
	}
}
