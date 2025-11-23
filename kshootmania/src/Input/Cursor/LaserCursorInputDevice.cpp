#include "LaserCursorInputDevice.hpp"

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

	// レーザーカーソル移動量を取得して加算
	const double deltaCursorX = KeyConfig::LaserDeltaCursorX(m_laneIdx, buttonL, buttonR, Scene::DeltaTime());
	m_accumulatedDelta += deltaCursorX;

	// 累積カーソル移動量がしきい値を超えたらカーソル移動
	if (Abs(m_accumulatedDelta) >= kThreshold)
	{
		const int32 steps = static_cast<int32>(m_accumulatedDelta / kThreshold);
		m_deltaCursor = steps;
		m_accumulatedDelta -= steps * kThreshold;
	}
}

int32 LaserCursorInputDevice::deltaCursor() const
{
	return m_deltaCursor;
}
