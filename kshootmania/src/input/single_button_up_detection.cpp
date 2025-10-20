#include "single_button_up_detection.hpp"

void SingleButtonUpDetection::update(KeyConfig::Button targetButton)
{
	// 対象ボタンが押された瞬間、他のボタンが押されていれば記録
	if (KeyConfig::Down(targetButton))
	{
		bool otherButtonPressed = false;
		for (KeyConfig::Button otherButton = 0; otherButton < KeyConfig::kButtonEnumCount; ++otherButton)
		{
			if (otherButton == targetButton)
			{
				continue;
			}

			if (KeyConfig::Pressed(otherButton))
			{
				otherButtonPressed = true;
				break;
			}
		}

		m_otherButtonPressed = otherButtonPressed;
	}

	// 対象ボタンを押している間に他のボタンが押されたら記録
	if (KeyConfig::Pressed(targetButton))
	{
		for (KeyConfig::Button otherButton = 0; otherButton < KeyConfig::kButtonEnumCount; ++otherButton)
		{
			if (otherButton == targetButton)
			{
				continue;
			}

			if (KeyConfig::Pressed(otherButton))
			{
				m_otherButtonPressed = true;
				break;
			}
		}
	}

	// 対象ボタンを離したら状態をリセット
	if (KeyConfig::Up(targetButton))
	{
		m_otherButtonPressed = false;
	}
}

bool SingleButtonUpDetection::up(KeyConfig::Button targetButton) const
{
	return KeyConfig::Up(targetButton) && !m_otherButtonPressed;
}

void SingleButtonUpDetection::reset()
{
	m_otherButtonPressed = false;
}
