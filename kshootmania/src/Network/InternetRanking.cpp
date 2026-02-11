#include "InternetRanking.hpp"
#include <Siv3D.hpp>

namespace InternetRanking
{
	Co::Task<bool> SubmitScore(const MusicGame::PlayResult& playResult)
	{
		// TODO: Implement actual HTTP request
		// Example payload preparation:
		/*
		ScoreData data;
		data.score = playResult.score;
		data.maxCombo = playResult.maxCombo;
		data.criticalCount = playResult.comboStats.critical;
		data.nearCount = playResult.comboStats.totalNear();
		data.errorCount = playResult.comboStats.error;

		// const URL url = U"https://api.your-server.com/submit";
		// const auto response = SimpleHTTP::Post(url, JSON::From(data));
		*/

		Logger << U"[ksm info] InternetRanking::SubmitScore: Submitting score {}..."_fmt(playResult.score);

		co_await Co::Delay(1.0s);

		Logger << U"[ksm info] InternetRanking::SubmitScore: Score submitted successfully.";
		co_return true;
	}
}
