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

		int32 seDelayMs = 0;

		int32 visualOffsetMs = 0;

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

		// visualOffsetを考慮したglobalOffsetを取得
		int32 effectiveGlobalOffsetMs() const
		{
			return globalOffsetMs - visualOffsetMs;
		}

		// visualOffsetを考慮したinputDelayを取得(オートプレイ時は判定調整なし)
		int32 effectiveInputDelayMs() const
		{
			if (isAutoPlay)
			{
				return visualOffsetMs;
			}
			else
			{
				return inputDelayMs + visualOffsetMs;
			}
		}

		// オートプレイ時は判定調整なし
		int32 effectiveLaserInputDelayMs() const
		{
			return isAutoPlay ? 0 : laserInputDelayMs;
		}

		// visualOffsetを考慮したseDelayを取得
		int32 effectiveSEDelayMs() const
		{
			return seDelayMs + visualOffsetMs;
		}
	};
}
