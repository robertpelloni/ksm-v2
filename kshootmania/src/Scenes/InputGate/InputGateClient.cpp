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
			AsyncHTTPTask task = SimpleHTTP::LoadAsync(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			if (task.getResponse().isOK())
			{
				const Blob blob = task.getBlob();
				const JSON json = JSON::Parse(TextReader{ MemoryViewReader{ blob.data(), blob.size() } }.readAll());
				if (json && json.isArray())
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

	Co::Task<Array<RankingEntry>> InputGateClient::fetchRanking(StringView songId, int32 difficulty)
	{
		const String apiBaseUrl = ConfigIni::GetString(ConfigIni::Key::kInputGateUrl, U"");

		if (apiBaseUrl.isEmpty())
		{
			// モックモード
			co_await Co::Delay(0.3s);

			Array<RankingEntry> entries;
			for (int32 i = 0; i < 20; ++i)
			{
				entries.push_back({
					i + 1,
					U"Player_{}"_fmt(i + 1),
					10000000 - (i * 15000) - Random(0, 5000),
					(i % 3 == 0) ? U"Gamepad" : U"Keyboard",
					U"2026-02-20"
				});
			}

			co_return entries;
		}
		else
		{
			const URL url = apiBaseUrl + U"/ranking.json?song={}&diff={}"_fmt(songId, difficulty);

			AsyncHTTPTask task = SimpleHTTP::LoadAsync(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			if (task.getResponse().isOK())
			{
				const Blob blob = task.getBlob();
				const JSON json = JSON::Parse(TextReader{ MemoryViewReader{ blob.data(), blob.size() } }.readAll());
				if (json && json.isArray())
				{
					Array<RankingEntry> entries;
					for (const auto& item : json.arrayView())
					{
						entries.push_back({
							static_cast<int32>(item[U"rank"].getOr<int64>(0)),
							item[U"player_name"].getString(),
							static_cast<int32>(item[U"score"].getOr<int64>(0)),
							item[U"play_style"].getString(),
							item[U"date"].getString()
						});
					}
					co_return entries;
				}
			}

			Logger << U"[ksm error] Failed to fetch ranking from {}"_fmt(url);
			co_return Array<RankingEntry>();
		}
	}

	Co::Task<int32> InputGateClient::downloadSong(StringView url, FilePathView savePath, std::function<void(double)> progressCallback)
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
			co_return static_cast<int32>(DownloadResult::Success);
		}
		else
		{
			// 実際のダウンロード
			co_return static_cast<int32>(co_await DownloadTask::Download(url, savePath, progressCallback));
		}
	}

	Co::Task<UpdateInfo> InputGateClient::checkUpdate()
	{
		UpdateInfo info;

		// 現在のバージョンを取得
		// リソースフォルダまたは実行ファイルディレクトリのVERSIONファイルを読む
		FilePath versionPath = FsUtils::GetResourcePath(U"VERSION");
		if (!FileSystem::Exists(versionPath))
		{
			// Fallback: Check local directory
			versionPath = U"VERSION";
		}

		if (FileSystem::Exists(versionPath))
		{
			TextReader reader{ versionPath };
			if (reader)
			{
				info.currentVersion = reader.readAll().trimmed();
			}
		}

		if (info.currentVersion.isEmpty())
		{
			info.currentVersion = U"Unknown";
		}

		const String apiBaseUrl = ConfigIni::GetString(ConfigIni::Key::kInputGateUrl, U"");

		if (apiBaseUrl.isEmpty())
		{
			// Mock: 50% chance to have update or always have update if version matches specific mock
			// For testing UI, let's say there is an update if current version contains "alpha"
			// But let's verify if current is alpha21, update to alpha22

			co_await Co::Delay(0.5s);

			info.hasUpdate = true;
			info.latestVersion = U"2.0.0-alpha24"; // Next mock version
			info.downloadUrl = U"http://example.com/update.zip";
			info.patchNotes = U"Mock Update:\n- New Input Gate features\n- Bug fixes";

			// If current is same or newer, no update
			if (info.currentVersion == info.latestVersion)
			{
				info.hasUpdate = false;
			}
		}
		else
		{
			const URL url = apiBaseUrl + U"/version.json";
			AsyncHTTPTask task = SimpleHTTP::LoadAsync(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			if (task.getResponse().isOK())
			{
				const Blob blob = task.getBlob();
				const JSON json = JSON::Parse(TextReader{ MemoryViewReader{ blob.data(), blob.size() } }.readAll());
				if (json)
				{
					info.latestVersion = json[U"latest_version"].getString();
				info.downloadUrl = json[U"download_url"].getString();
					info.patchNotes = json[U"patch_notes"].getString();

					if (info.latestVersion != info.currentVersion) // Simple string compare for now
					{
						// TODO: Semver compare?
						info.hasUpdate = true;
					}
				}
			}
			else
			{
				Logger << U"[ksm error] Failed to fetch version info from {}"_fmt(url);
			}
		}

		co_return info;
	}
}
