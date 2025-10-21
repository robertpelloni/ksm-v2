#include "scoring_status.hpp"

namespace MusicGame::Judgment
{
	namespace
	{
		// ゲージ種類に応じた上昇率倍率を取得
		double GetGaugeIncreaseRate(GaugeType gaugeType)
		{
			switch (gaugeType)
			{
			case GaugeType::kEasyGauge:
				return kGaugeIncreaseRateEasy;
			case GaugeType::kNormalGauge:
				return kGaugeIncreaseRateNormal;
			case GaugeType::kHardGauge:
				return kGaugeIncreaseRateHard;
			default:
				throw Error{ U"GetGaugeIncreaseRate(): Invalid gauge type (gaugeType:{})"_fmt(static_cast<std::underlying_type_t<GaugeType>>(gaugeType)) };
			}
		}

		// ゲージ種類に応じた下降率倍率を取得
		double GetGaugeDecreaseRate(GaugeType gaugeType)
		{
			switch (gaugeType)
			{
			case GaugeType::kEasyGauge:
				return kGaugeDecreaseRateEasy;
			case GaugeType::kNormalGauge:
				return kGaugeDecreaseRateNormal;
			case GaugeType::kHardGauge:
				return kGaugeDecreaseRateHard;
			default:
				throw Error{ U"GetGaugeDecreaseRate(): Invalid gauge type (gaugeType:{})"_fmt(static_cast<std::underlying_type_t<GaugeType>>(gaugeType)) };
			}
		}
	}

	void ScoringStatus::addGaugeValue(int32 add)
	{
		const double rate = GetGaugeIncreaseRate(m_gaugeType);
		const int32 adjustedAdd = static_cast<int32>(add * rate);
		m_gaugeValue = Min(m_gaugeValue + adjustedAdd, m_gaugeValueMax);
	}

	void ScoringStatus::subtractGaugeValue(int32 sub)
	{
		const double rate = GetGaugeDecreaseRate(m_gaugeType);
		const int32 adjustedSub = static_cast<int32>(sub * rate);
		m_gaugeValue = Max(m_gaugeValue - adjustedSub, 0);
	}

	ScoringStatus::ScoringStatus(int32 scoreValueMax, int32 gaugeValueMax, GaugeType gaugeType)
		: m_scoreValueMax(scoreValueMax)
		, m_gaugeValueMax(gaugeValueMax)
		, m_gaugeType(gaugeType)
	{
	}

	void ScoringStatus::onChipOrLaserSlamJudgment(Judgment::JudgmentResult result)
	{
		m_comboStatus.processJudgmentResult(result);

		switch (result)
		{
		case Judgment::JudgmentResult::kCritical:
			m_scoreValue += Judgment::kScoreValueCritical;
			addGaugeValue(kGaugeValueChip);
			break;

		case Judgment::JudgmentResult::kNearFast:
		case Judgment::JudgmentResult::kNearSlow:
			m_scoreValue += Judgment::kScoreValueNear;
			addGaugeValue(kGaugeValueChipNear);
			break;

		case Judgment::JudgmentResult::kError:
			subtractGaugeValue(static_cast<int32>(m_gaugeValueMax * kGaugeDecreasePercentByChipError / 100));
			break;

		default:
			assert(false && "Invalid JudgmentResult in doChipJudgment");
			break;
		}
	}

	void ScoringStatus::onLongOrLaserLineJudgment(Judgment::JudgmentResult result)
	{
		m_comboStatus.processJudgmentResult(result);

		switch (result)
		{
		case Judgment::JudgmentResult::kCritical:
			m_scoreValue += Judgment::kScoreValueCritical;
			addGaugeValue(kGaugeValueLong);
			break;

		case Judgment::JudgmentResult::kError:
			subtractGaugeValue(static_cast<int32>(m_gaugeValueMax * kGaugeDecreasePercentByLongError / 100));
			break;

		default:
			assert(false && "Invalid JudgmentResult in doLongJudgment");
			break;
		}
	}

	int32 ScoringStatus::score() const
	{
		if (m_scoreValueMax == 0)
		{
			return 0;
		}
		return static_cast<int32>(static_cast<int64>(kScoreMax) * m_scoreValue / m_scoreValueMax);
	}

	double ScoringStatus::gaugePercentage() const
	{
		if (m_gaugeValueMax == 0)
		{
			return 0.0;
		}
		return 100.0 * m_gaugeValue / m_gaugeValueMax;
	}

	int32 ScoringStatus::combo() const
	{
		return m_comboStatus.combo();
	}

	int32 ScoringStatus::maxCombo() const
	{
		return m_comboStatus.maxCombo();
	}

	const ComboStats& ScoringStatus::comboStats() const
	{
		return m_comboStatus.stats();
	}

	bool ScoringStatus::isNoError() const
	{
		return m_comboStatus.isNoError();
	}

	int32 ScoringStatus::totalJudgedCombo() const
	{
		return m_comboStatus.totalJudgedCombo();
	}
}
