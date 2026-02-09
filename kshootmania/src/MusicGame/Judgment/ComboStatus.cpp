#include "ComboStatus.hpp"

namespace MusicGame::Judgment
{
	void ComboStatus::processJudgmentResult(JudgmentResult result, double diffSec)
	{
		if (result == JudgmentResult::kCritical || result == JudgmentResult::kNearFast || result == JudgmentResult::kNearSlow)
		{
			// Add deviation info
			// Note: We only add deviation if it's not exactly zero or if it's a valid judgment.
			// Ideally we want to track tendency even for criticals.
			// diffSec: FAST < 0, SLOW > 0.
			// However, usually "Fast" means "Pressed Early", so diffSec (NoteTime - InputTime) would be Positive.
			// Let's verify ButtonLaneJudgment.cpp:
			// const double diffSec = sec - currentTimeSec;
			// isFast = currentTimeSec < sec; => sec - currentTimeSec > 0.
			// So FAST means diffSec > 0.
			// SLOW means diffSec < 0.
			// Wait, let's double check.
			// If I press at 1.0s for a note at 1.1s (Early/Fast): diff = 1.1 - 1.0 = +0.1s.
			// If I press at 1.2s for a note at 1.1s (Late/Slow): diff = 1.1 - 1.2 = -0.1s.
			// So Positive = Fast/Early, Negative = Slow/Late.

			// For InputDelay adjustment:
			// If I am consistently Fast (+0.1s), it means I am hitting early.
			// To correct this, I need to make the note appear *later*? No.
			// If I hit early, it means the audio is perceived earlier than the visual? Or visual is late?
			// Usually InputDelay shifts the *judgment window*.
			// If InputDelay is +10ms, we subtract 10ms from the input time (or add to note time).
			// If I am Fast (+), I am hitting before the note.
			// If we add positive offset to InputDelay, we are saying "Input is actually later than the hardware event".
			// So if I hit at 1.0s (timestamp), and InputDelay is +0.1s, the game thinks I hit at 1.1s.
			// This matches the note at 1.1s.
			// So: Positive Average Diff => Recommended InputDelay += Diff.

			m_stats.totalDeviationSec += diffSec;
			m_stats.deviationCount++;
		}

		switch (result)
		{
		case JudgmentResult::kCritical:
			++m_combo;
			++m_courseCombo;
			m_maxCombo = Max(m_maxCombo, m_combo);
			++m_stats.critical;
			break;

		case JudgmentResult::kNearFast:
			++m_combo;
			++m_courseCombo;
			m_maxCombo = Max(m_maxCombo, m_combo);
			++m_stats.nearFast;
			break;

		case JudgmentResult::kNearSlow:
			++m_combo;
			++m_courseCombo;
			m_maxCombo = Max(m_maxCombo, m_combo);
			++m_stats.nearSlow;
			break;

		case JudgmentResult::kError:
			m_combo = 0;
			m_courseCombo = 0;
			m_courseIsNoError = false;
			++m_stats.error;
			break;

		default:
			assert(false && "Invalid JudgmentResult in processJudgmentResult");
			break;
		}
	}

	int32 ComboStatus::combo() const
	{
		return m_combo;
	}

	int32 ComboStatus::maxCombo() const
	{
		return m_maxCombo;
	}

	const ComboStats& ComboStatus::stats() const
	{
		return m_stats;
	}

	bool ComboStatus::isNoError() const
	{
		return m_stats.error == 0;
	}

	int32 ComboStatus::totalJudgedCombo() const
	{
		return m_stats.totalJudgedCombo();
	}

	int32 ComboStatus::courseCombo() const
	{
		return m_courseCombo;
	}

	bool ComboStatus::courseIsNoError() const
	{
		return m_courseIsNoError;
	}
}
