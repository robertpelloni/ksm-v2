#pragma once
#include "MusicGame/PlayResult.hpp"

namespace InternetRanking
{
	// スコア送信用のデータ構造
	struct ScoreData
	{
		String songId; // Chart ID or Hash
		String songTitle;
		String songArtist;
		int32 score = 0;
		int32 maxCombo = 0;
		int32 criticalCount = 0;
		int32 nearCount = 0;
		int32 errorCount = 0;
		int32 gaugeValue = 0;
		// Optional: Gauge Type, Modifiers
	};

	/// @brief スコアを送信する(モック)
	/// @param playResult プレイ結果
	/// @return 送信に成功したかどうか(Task)
	Co::Task<bool> SubmitScore(const MusicGame::PlayResult& playResult);
}
