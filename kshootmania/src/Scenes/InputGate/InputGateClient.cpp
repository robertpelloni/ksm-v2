#include "InputGateClient.hpp"
#include "Ini/ConfigIni.hpp"
#include <Siv3D.hpp>

namespace InputGate
{
	Co::Task<Array<SongInfo>> InputGateClient::fetchSongList()
	{
		const String apiBaseUrl = ConfigIni::GetString(ConfigIni::Key::kInputGateUrl, U"");

		if (apiBaseUrl.isEmpty())
		{
			// モックモード
			co_await Co::Delay(0.5s);

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
			// const URL url = apiBaseUrl + U"/list";
			// const auto response = SimpleHTTP::Get(url);
			// if (response.isOK()) { ... }
			Logger << U"[ksm info] InputGateClient: Fetching from {} (Not implemented)"_fmt(apiBaseUrl);
			co_await Co::Delay(0.1s);
			co_return Array<SongInfo>();
		}
	}

	Co::Task<bool> InputGateClient::downloadSong(StringView url, FilePathView savePath, std::function<void(double)> progressCallback)
	{
		// URLが空の場合はモックとみなす(またはエラー)
		if (url.isEmpty() || url.includes(U"example.com"))
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
			// Note: Asynchronous download with progress requires threading or polling
			Logger << U"[ksm info] InputGateClient: Downloading from {} to {} (Mocked)"_fmt(url, savePath);
			co_await Co::Delay(1.0s);
			co_return true;
		}
	}
}
