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

		const URL url = apiBaseUrl.isEmpty() ? U"http://localhost:3000/api/input_gate/list" : (apiBaseUrl + U"/list");

		// 非同期でJSONを取得
		AsyncHTTPTask task = SimpleHTTP::LoadAsync(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			const auto response = task.getResponse();
			if (response.isOK())
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

		const URL url = apiBaseUrl.isEmpty() ? U"http://localhost:3000/api/input_gate/ranking?song={}&diff={}"_fmt(songId, difficulty) : (apiBaseUrl + U"/ranking?song={}&diff={}"_fmt(songId, difficulty));

		AsyncHTTPTask task = SimpleHTTP::LoadAsync(url);

			while (!task.isReady())
			{
				co_await Co::NextFrame();
			}

			const auto response = task.getResponse();
			if (response.isOK())
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

			const auto response = task.getResponse();
			if (response.isOK())
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
