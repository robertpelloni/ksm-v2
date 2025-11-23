#include "AnalogStickXYLaserInput.hpp"

namespace
{
	// デフォルトの入力感度設定値
	constexpr int32 kDefaultSensitivity = 50;

	// 入力感度の係数の基準値
	constexpr double kBaseSensitivityScale = 50.0 / 400.0;

	// 循環検出のしきい値
	constexpr double kWrapThreshold = 0.5;

	// 微弱な動きを無視するしきい値
	constexpr double kDeadZone = 25.0 / 65536.0;
}

AnalogStickXYLaserInput::AnalogStickXYLaserInput(int32 laneIdx)
	: m_laneIdx(laneIdx)
{
}

double AnalogStickXYLaserInput::getDeltaCursorX(double deltaTimeSec)
{
	// 左右レーザーのボタン配置を入れ替えるフラグ
	const bool swapLR = ConfigIni::GetBool(ConfigIni::Key::kSwapLaserLR, false);

	// 軸インデックスを計算(軸0=左スティックX、軸1=左スティックY)
	const int32 axisIdx = 0 + (m_laneIdx ^ (swapLR ? 1 : 0));

	// 接続されているゲームパッドから軸の値を取得
	double currentAxisValue = 0.0;
	bool foundGamepad = false;

	for (int32 playerIdx = 0; playerIdx < static_cast<int32>(Gamepad.MaxPlayerCount); ++playerIdx)
	{
		if (const auto& gamepad = Gamepad(playerIdx))
		{
			if (axisIdx < static_cast<int32>(gamepad.axes.size()))
			{
				const double rawAxisValue = gamepad.axes[axisIdx];
				// Siv3Dのaxes値は-1.0～1.0の範囲なので0.0～1.0に正規化
				currentAxisValue = (rawAxisValue + 1.0) * 0.5;
				foundGamepad = true;
				break;
			}
		}
	}

	if (!foundGamepad)
	{
		return 0.0;
	}

	// 初回フレームは差分計算できないので0を返す
	if (m_isFirstFrame)
	{
		m_prevAxisValue = currentAxisValue;
		m_isFirstFrame = false;
		return 0.0;
	}

	// 前フレームとの差分を計算
	double delta = currentAxisValue - m_prevAxisValue;

	// 循環処理
	if (Abs(delta) > kWrapThreshold)
	{
		delta -= Sign(delta) * 1.0;
	}

	// 前フレームの値を更新
	m_prevAxisValue = currentAxisValue;

	// 感度調整
	const int32 sensitivity = ConfigIni::GetInt(ConfigIni::Key::kLaserSignalSensitivity, kDefaultSensitivity);
	const double sensitivityScale = kBaseSensitivityScale * (sensitivity / static_cast<double>(kDefaultSensitivity));
	delta *= sensitivityScale;

	// 微細な動きを無視
	if (Abs(delta) < kDeadZone)
	{
		return 0.0;
	}

	return delta;
}

bool AnalogStickXYLaserInput::reconstructionNeeded() const
{
	const int32 laserInputType = ConfigIni::GetInt(ConfigIni::Key::kLaserInputType, ConfigIni::Value::LaserInputType::kKeyboard);
	return laserInputType != ConfigIni::Value::LaserInputType::kAnalogStickXY;
}
