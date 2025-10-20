#pragma once
#include "key_config.hpp"

class SingleButtonUpDetection
{
private:
	bool m_otherButtonPressed = false;

public:
	void update(KeyConfig::Button targetButton);

	bool up(KeyConfig::Button targetButton) const;

	void reset();
};
