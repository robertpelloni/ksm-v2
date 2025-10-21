#pragma once

namespace MusicGame
{
	constexpr Duration kTimeSecBeforeStart = 3.4s;
	constexpr Duration kTimeSecBeforeStartMovie = 4.4s;

	constexpr Duration TimeSecBeforeStart(bool movie)
	{
		return movie ? kTimeSecBeforeStartMovie : kTimeSecBeforeStart;
	}

	const std::array<KeyConfig::Button, kson::kNumBTLanesSZ> kBTButtons = {
		KeyConfig::kBT_A,
		KeyConfig::kBT_B,
		KeyConfig::kBT_C,
		KeyConfig::kBT_D,
	};

	const std::array<KeyConfig::Button, kson::kNumFXLanesSZ> kFXButtons = {
		KeyConfig::kFX_L,
		KeyConfig::kFX_R,
	};

	const std::array<std::array<KeyConfig::Button, 2U>, kson::kNumFXLanesSZ> kLaserButtons = { {
		{ KeyConfig::kLeftLaserL, KeyConfig::kLeftLaserR },
		{ KeyConfig::kRightLaserL, KeyConfig::kRightLaserR },
	} };

	constexpr kson::Pulse kPastPulse = -100000000;

	constexpr int32 kScoreMax = 10000000;

	constexpr int32 kGaugeValueChip = 200;
	constexpr int32 kGaugeValueChipNear = 50;
	constexpr int32 kGaugeValueLong = 50;

	constexpr double kGaugePercentageThreshold = 70.0;
	constexpr double kGaugePercentageThresholdHardWarning = 30.0;

	constexpr double kGaugeDecreasePercentByChipError = 2.0;
	constexpr double kGaugeDecreasePercentByLongError = 0.5;

	constexpr double kTiltRadians = 10_deg;

	using IsAutoPlayYN = YesNo<struct IsAutoPlayYN_tag>;

	// Turn変換テーブル
	struct TurnTable
	{
		std::array<std::size_t, kson::kNumBTLanesSZ> btLaneTable; // BTレーン変換テーブル

		bool invertFXLane; // FXレーン左右反転

		bool invertLaserLane; // LASERレーン左右反転

		bool invertLaserValue; // LASER値左右反転(カメラエフェクト左右反転にも使用)
	};

	// TurnMode から TurnTable を生成
	inline TurnTable MakeTurnTable(TurnMode turnMode)
	{
		if (turnMode == TurnMode::kNormal)
		{
			return TurnTable{
				.btLaneTable = { 0, 1, 2, 3 },
				.invertFXLane = false,
				.invertLaserLane = false,
				.invertLaserValue = false,
			};
		}
		else if (turnMode == TurnMode::kMirror)
		{
			return TurnTable{
				.btLaneTable = { 3, 2, 1, 0 }, // D,C,B,A
				.invertFXLane = true, // R,L
				.invertLaserLane = true, // R,L
				.invertLaserValue = true, // 1.0 - 値
			};
		}
		else // TurnMode::kRandom
		{
			// BTレーンのランダムテーブル生成
			std::array<std::size_t, kson::kNumBTLanesSZ> btLaneTable = { 0, 1, 2, 3 };
			std::shuffle(btLaneTable.begin(), btLaneTable.end(), GetDefaultRNG());

			// FX/LASERレーンのランダム左右反転
			const bool invertFXLane = Random(0, 1) == 1;
			const bool invertLaserLane = Random(0, 1) == 1;

			return TurnTable{
				.btLaneTable = btLaneTable,
				.invertFXLane = invertFXLane,
				.invertLaserLane = invertLaserLane,
				.invertLaserValue = invertLaserLane, // レーザー反転と同じ
			};
		}
	}
}
