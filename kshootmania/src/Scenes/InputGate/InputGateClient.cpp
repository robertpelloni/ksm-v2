#include "InputGateClient.hpp"
#include "Ini/ConfigIni.hpp"
#include "Common/FsUtils.hpp"
#include "DownloadTask.hpp"
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
			const URL url = apiBaseUrl + U"/list.json"; // サーバー実装に合わせて変更

			// 非同期でJSONを取得
			AsyncHTTPTask task = SimpleHTTP::CreateGetTask(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			if (task.getResponse().isOK())
			{
				const JSON json = task.getAsJSON();
				if (json.isArray())
				{
					Array<SongInfo> songs;
					for (const auto& item : json.arrayView())
					{
						songs.push_back({
							item[U"id"].getString(),
							item[U"title"].getString(),
							item[U"artist"].getString(),
							item[U"jacket_url"].getString(),
							item[U"download_url"].getString(),
							item[U"preview_url"].getString(),
							item[U"hash"].getString(),
							static_cast<int32>(item[U"size"].getOr<int64>(0))
						});
					}
					co_return songs;
				}
			}

			Logger << U"[ksm error] Failed to fetch song list from {}"_fmt(url);
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
			// 実際のダウンロード
			return DownloadTask::Download(url, savePath, progressCallback);
		}
	}
}
