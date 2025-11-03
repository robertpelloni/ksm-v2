#pragma once
#include <Siv3D.hpp>
#include <NocoUI.hpp>
#include <CoTaskLib.hpp>

// スコア更新時に表示されるNewRecordパネル
class ResultNewRecordPanel
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;

	bool m_isVisible = false;

	Stopwatch m_fadeInStopwatch;

	Stopwatch m_fadeOutStopwatch;

	static constexpr double kFadeInDelay = 0.35;
	static constexpr double kTweenDuration = 0.15;

public:
	explicit ResultNewRecordPanel(std::shared_ptr<noco::Canvas> canvas);

	void setValue(int32 oldScore, int32 newScore);

	bool isVisible() const;

	void startDisplay();

	void startRedisplay();

	Co::Task<void> waitForFadeIn();

	void startFadeOut();

	Co::Task<void> waitForFadeOut();
};
