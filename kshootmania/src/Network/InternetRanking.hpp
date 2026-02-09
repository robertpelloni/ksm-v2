#pragma once
#include "MusicGame/PlayResult.hpp"

namespace InternetRanking
{
	/// @brief スコアを送信する(モック)
	/// @param playResult プレイ結果
	/// @return 送信に成功したかどうか(Task)
	Co::Task<bool> SubmitScore(const MusicGame::PlayResult& playResult);
}
