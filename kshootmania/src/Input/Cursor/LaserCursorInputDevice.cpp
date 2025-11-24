#include "LaserCursorInputDevice.hpp"

namespace
{
	// カーソル移動のしきい値
	constexpr double kThresholdFirst = 0.5;
	constexpr double kThresholdSecond = 0.7;
	constexpr double kThreshold = 0.4;
	constexpr double kThresholdOver10 = 0.2;

	// 入力がない状態が続いた場合にリセットするまでの時間
	constexpr Duration kIdleResetTime = 0.3s;

	// 入力の有無を判定する最小しきい値
	constexpr double kInputDetectionThreshold = 0.001;
}

LaserCursorInputDevice::LaserCursorInputDevice(int32 laneIdx)
	: m_laneIdx(laneIdx)
{
}

LaserCursorInputDevice::~LaserCursorInputDevice() = default;

void LaserCursorInputDevice::update()
{
	m_deltaCursor = 0;

	// レーンインデックスからボタンを決定
	const KeyConfig::Button buttonL = m_laneIdx == 0 ? KeyConfig::kLeftLaserL : KeyConfig::kRightLaserL;
	const KeyConfig::Button buttonR = m_laneIdx == 0 ? KeyConfig::kLeftLaserR : KeyConfig::kRightLaserR;

	double prevAccumulatedDelta = m_accumulatedDelta;

	// レーザーカーソル移動量を取得して加算
	const double deltaCursorX = KeyConfig::LaserDeltaCursorX(m_laneIdx, buttonL, buttonR, Scene::DeltaTime());
	m_accumulatedDelta += deltaCursorX;

	if (Sign(prevAccumulatedDelta) != Sign(m_accumulatedDelta) && prevAccumulatedDelta != 0.0 && m_accumulatedDelta != 0.0)
	{
		// 累積移動量の符号が変わったら移動回数をリセット
		m_movementCount = 0;
		Print << U"LaserCursorInputDevice: Idle reset (laneIdx=" << m_laneIdx << U")" << Time::GetMillisec();
	}

	if (Abs(deltaCursorX) > kInputDetectionThreshold)
	{
		// 入力があれば入力なし時間をリセット
		m_idleStopwatch.restart();
	}
	else if (m_idleStopwatch.elapsed() > kIdleResetTime)
	{
		// 一定時間入力がなければ移動回数と累積値をリセット
		m_movementCount = 0;
		m_accumulatedDelta = 0.0;
	}

	// 移動回数に応じてしきい値を変える
	// (1回のみのカーソル移動をしやすくするため)
	double threshold;
	if (m_movementCount == 0)
	{
		threshold = kThresholdFirst;
	}
	else if (m_movementCount == 1)
	{
		threshold = kThresholdSecond;
	}
	else if (m_movementCount > 10)
	{
		threshold = kThresholdOver10;
	}
	else
	{
		threshold = kThreshold;
	}

	// 累積カーソル移動量がしきい値を超えたらカーソル移動
	if (Abs(m_accumulatedDelta) >= threshold)
	{
		if (m_movementCount < 2)
		{
			m_deltaCursor = m_accumulatedDelta < 0.0 ? -1 : 1;
			m_accumulatedDelta = 0.0;
			m_movementCount++;
		}
		else
		{
			const int32 steps = static_cast<int32>(m_accumulatedDelta / threshold);
			m_deltaCursor = steps;
			m_accumulatedDelta -= steps * threshold;
			m_movementCount++;
		}
	}
}

int32 LaserCursorInputDevice::deltaCursor() const
{
	return m_deltaCursor;
}
