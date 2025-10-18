#pragma once

namespace MusicGame
{
	struct PlayOption
	{
		IsAutoPlayYN isAutoPlay = IsAutoPlayYN::No;

		GaugeType gaugeType = GaugeType::kNormalGauge;

		int32 globalOffsetMs = 0;

		int32 inputDelayMs = 0;

		int32 laserInputDelayMs = 0;

		JudgmentPlayMode btJudgmentPlayMode() const
		{
			return isAutoPlay ? JudgmentPlayMode::kAuto : JudgmentPlayMode::kOn;
		}

		JudgmentPlayMode fxJudgmentPlayMode() const
		{
			return isAutoPlay ? JudgmentPlayMode::kAuto : JudgmentPlayMode::kOn;
		}

		JudgmentPlayMode laserJudgmentPlayMode() const
		{
			return isAutoPlay ? JudgmentPlayMode::kAuto : JudgmentPlayMode::kOn;
		}
	};
}
