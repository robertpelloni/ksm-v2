#pragma once
#include "MusicGame/GameDefines.hpp"
#include "Judgment/ComboStatus.hpp"
#include "PlayOption.hpp"

namespace MusicGame
{
	struct PlayResult
	{
		int32 score = 0;

		int32 maxCombo = 0;

		int32 totalCombo = 0;

		Judgment::ComboStats comboStats;

		PlayOption playOption;

		double gaugePercentage = 0.0;

		double gaugePercentageForGrade = 0.0;

		int32 gaugeValue = 0; // 内部ゲージ値(コースモード時の引き継ぎ用)

		double chartTimeProgress = 0.0; // 譜面進行率(0.0〜1.0、コースモードでの途中落ち時のAchievementRate計算用)

		IsHardFailedYN isHardFailed = IsHardFailedYN::No; // HARDゲージ/コースモードで途中落ちしたかどうか

		bool isAborted() const;

		Achievement achievement() const;

		Grade grade() const;

		int32 gaugePercentForHighScore() const;
	};
}
