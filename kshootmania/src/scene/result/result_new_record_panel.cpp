#include "result_new_record_panel.hpp"

namespace
{
	String FormatScoreDelta(int32 delta)
	{
		return U"{:08d}"_fmt(delta);
	}
}

ResultNewRecordPanel::ResultNewRecordPanel(std::shared_ptr<noco::Canvas> canvas)
	: m_canvas(canvas)
{
}

void ResultNewRecordPanel::setValue(int32 oldScore, int32 newScore)
{
	const bool shouldBeVisible = newScore > oldScore;

	if (shouldBeVisible)
	{
		const int32 scoreDelta = newScore - oldScore;
		m_canvas->setParamValue(U"overlay_newRecordNumber", FormatScoreDelta(scoreDelta));
	}

	m_isVisible = shouldBeVisible;
}

bool ResultNewRecordPanel::isVisible() const
{
	return m_isVisible;
}

void ResultNewRecordPanel::startDisplay()
{
	m_canvas->setParamValue(U"overlay_visible", true);
	m_canvas->setTweenActiveByTag(U"out_newRecord", false);
	m_canvas->setTweenActiveByTag(U"in_newRecord", true);
	m_fadeInStopwatch.restart();
}

void ResultNewRecordPanel::startRedisplay()
{
	m_canvas->setParamValue(U"overlay_visible", true);
	m_canvas->setTweenActiveByTag(U"out_newRecord", false);
	m_canvas->setTweenActiveByTag(U"inAgain_newRecord", true);
	m_fadeInStopwatch.restart();
}

Co::Task<void> ResultNewRecordPanel::waitForFadeIn()
{
	co_await Co::WaitUntil([this]
	{
		return m_fadeInStopwatch.sF() >= kFadeInDelay + kTweenDuration;
	});
}

void ResultNewRecordPanel::startFadeOut()
{
	m_canvas->setTweenActiveByTag(U"in_newRecord", false);
	m_canvas->setTweenActiveByTag(U"inAgain_newRecord", false);
	m_canvas->setTweenActiveByTag(U"out_newRecord", true);
	m_fadeOutStopwatch.restart();
}

Co::Task<void> ResultNewRecordPanel::waitForFadeOut()
{
	co_await Co::WaitUntil([this]
	{
		return m_fadeOutStopwatch.sF() >= kTweenDuration;
	});

	m_canvas->setParamValue(U"overlay_visible", false);
	m_isVisible = false;
}
