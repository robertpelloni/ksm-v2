#pragma once
#include <array>
#include "key_config.hpp"

class FXButtonUpDetection
{
private:
	static constexpr int32 kNumFXButtons = 2;

	std::array<bool, kNumFXButtons> m_otherFXButtonPressed = {};

	void updateButton(KeyConfig::Button targetButton, KeyConfig::Button otherFXButton);

public:
	void update();

	bool up(KeyConfig::Button targetButton) const;

	void reset();
};
