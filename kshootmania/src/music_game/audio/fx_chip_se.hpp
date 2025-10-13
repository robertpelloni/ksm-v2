#pragma once
#include "music_game/game_defines.hpp"
#include "music_game/game_status.hpp"
#include "kson/chart_data.hpp"
#include "kson/util/timing_utils.hpp"
#include "ksmaudio/ksmaudio.hpp"

namespace MusicGame::Audio
{
	class FXChipSE
	{
	private:
		std::unordered_map<std::string, ksmaudio::Sample> m_keySounds;

		std::array<double, kson::kNumFXLanesSZ> m_lastPlayedTimeSecs = { kPastTimeSec, kPastTimeSec };

	public:
		explicit FXChipSE(const kson::ChartData& chartData, FilePathView parentPath);

		void update(const kson::ChartData& chartData, const GameStatus& gameStatus);
	};
}
