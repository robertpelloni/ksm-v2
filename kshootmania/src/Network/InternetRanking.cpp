#include "InternetRanking.hpp"

namespace InternetRanking
{
	Co::Task<bool> SubmitScore(const MusicGame::PlayResult& playResult)
	{
		// TODO: Implement actual HTTP request
		// For now, simulate network delay and return success
		Logger << U"[ksm info] InternetRanking::SubmitScore: Submitting score {}..."_fmt(playResult.score);

		co_await Co::Delay(1.0s);

		Logger << U"[ksm info] InternetRanking::SubmitScore: Score submitted successfully.";
		co_return true;
	}
}
