#pragma once
#include "ILaserInputMethod.hpp"

/// @brief キーボード式レーザー入力
class KeyboardLaserInput : public ILaserInputMethod
{
private:
	const KeyConfig::Button m_buttonL;
	const KeyConfig::Button m_buttonR;

public:
	KeyboardLaserInput(KeyConfig::Button buttonL, KeyConfig::Button buttonR);

	[[nodiscard]]
	double getDeltaCursorX(double deltaTimeSec) override;

	[[nodiscard]]
	bool reconstructionNeeded() const override;
};
