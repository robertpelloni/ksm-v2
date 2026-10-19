#include "InternetRanking.hpp"
#include "Ini/ConfigIni.hpp"
#include <Siv3D.hpp>
#include <future>
#include <chrono>

namespace InternetRanking
{
	namespace
	{
		struct ScoreDataJson
		{
			int32 score;
			int32 maxCombo;
			int32 criticalCount;
			int32 nearCount;
			int32 errorCount;
			int32 gaugeValue;
			// Note: We might want more metadata like chart hash, but this is a start
		};

		void to_json(JSON& j, const ScoreDataJson& data)
		{
			j = JSON
			{
				{ U"score", data.score },
				{ U"maxCombo", data.maxCombo },
				{ U"criticalCount", data.criticalCount },
				{ U"nearCount", data.nearCount },
				{ U"errorCount", data.errorCount },
				{ U"gaugeValue", data.gaugeValue },
			};
		}
	}

	Co::Task<bool> SubmitScore(const MusicGame::PlayResult& playResult)
	{
		const String url = ConfigIni::GetString(ConfigIni::Key::kInternetRankingUrl, U"");

		if (url.isEmpty())
		{
			// URL not configured, treat as successful mock
			Logger << U"[ksm info] InternetRanking::SubmitScore: URL not configured (Mock mode). Submitting score {}..."_fmt(playResult.score);
			co_await Co::Delay(1.0s);
			Logger << U"[ksm info] InternetRanking::SubmitScore: Mock submission successful.";
			co_return true;
		}

		ScoreDataJson data
		{
			.score = playResult.score,
			.maxCombo = playResult.maxCombo,
			.criticalCount = playResult.comboStats.critical,
			.nearCount = playResult.comboStats.totalNear(),
			.errorCount = playResult.comboStats.error,
			.gaugeValue = playResult.gaugeValue,
		};

		// SimpleHTTP::Post is synchronous, so we might block the thread if we don't wrap it or use async HTTP client.
		// Since we are in a Co::Task, blocking the main thread (if this runs on main) is bad.
		// However, standard Siv3D HTTP might be blocking.
		// For now, we accept the risk or would use a thread if we had a thread pool.
		// But let's assume we want to keep it simple for this "Real Implementation" step.
		// A better approach in a game loop is to run this in a separate thread.

		// Note: Since we don't have a backend to test against, we'll log the attempt.
		Logger << U"[ksm info] InternetRanking::SubmitScore: Posting to {}..."_fmt(url);

		const URL actualUrl = url.isEmpty() ? U"http://localhost:3000/api/ranking/submit" : url;
		Logger << U"[ksm info] InternetRanking::SubmitScore: Posting to {}..."_fmt(actualUrl);

		const HashTable<String, String> headers = { { U"Content-Type", U"application/json" } };
		JSON json;
		to_json(json, data);
		const std::string jsonStr = json.formatUTF8();

		auto future = std::async(std::launch::async, [actualUrl, headers, jsonStr]() {
			return SimpleHTTP::Post(actualUrl, headers, jsonStr.data(), jsonStr.size());
		});

		while (future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		{
			co_await Co::NextFrame();
		}

		const auto response = future.get();
		if (response.isOK())
		{
			Logger << U"[ksm info] InternetRanking::SubmitScore: Score submission successful.";
			co_return true;
		}
		else
		{
			Logger << U"[ksm error] InternetRanking::SubmitScore: Failed to submit score. Status code: {}"_fmt(EnumToString(response.getStatusCode()));
			co_return false;
		}
	}
}
