#pragma once
#include "ILightingDriver.hpp"
#include "Drivers/MockLightingDriver.hpp"
#include "Drivers/HidLightingDriver.hpp"
#include "Ini/ConfigIni.hpp"

namespace Hardware::Lighting
{
	class LightingManager
	{
	private:
		std::unique_ptr<ILightingDriver> m_driver;
		LightingState m_currentState;

	public:
		void init()
		{
			// Read config
			const bool enabled = ConfigIni::GetBool(ConfigIni::Key::kInputGateUrl, true); // Reusing a key? No, need new key.
			// Actually let's use a specific key for lighting.
			// For now default to Mock if HID fails or disabled.

			// Try HID first if configured
			// int vid = ConfigIni::GetInt(U"ControllerVID", 0x1973);
			// int pid = ConfigIni::GetInt(U"ControllerPID", 0x2001);

			// Temporary hardcoded default for YuanCon/Generic
			// int vid = 0x1973;
			// int pid = 0x2001;

			// For safety in this environment, use Mock by default unless configured.
			// But I want to implement the HidDriver.

			// Read config
			const bool enabled = ConfigIni::GetBool(ConfigIni::Key::kInputGateUrl, true); // Reusing a key? No, need new key.
			// Ideally we have a dedicated key in ConfigIni for Lighting enabled

			// Try HID first if HIDAPI is available
#ifdef KSM_HIDAPI_ENABLED
			// Try to open standard controller (YuanCon default VID/PID for now)
			// In future, scan list or config
			auto hidDriver = std::make_unique<HidLightingDriver>(0x1973, 0x2001);
			if (hidDriver->init())
			{
				m_driver = std::move(hidDriver);
			}
			else
			{
				m_driver = std::make_unique<MockLightingDriver>();
				m_driver->init();
			}
#else
			m_driver = std::make_unique<MockLightingDriver>();
			m_driver->init();
#endif
		}

		void update(const LightingState& state)
		{
			m_currentState = state;
			if (m_driver)
			{
				m_driver->update(state);
			}
		}

		void shutdown()
		{
			if (m_driver)
			{
				m_driver->close();
				m_driver = nullptr;
			}
		}

		const LightingState& getState() const
		{
			return m_currentState;
		}
	};
}
