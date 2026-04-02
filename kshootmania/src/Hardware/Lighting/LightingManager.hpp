#pragma once
#include "ILightingDriver.hpp"
#include "Drivers/MockLightingDriver.hpp"
#include "Drivers/HidLightingDriver.hpp"
#include "Ini/ConfigIni.hpp"

namespace Hardware::Lighting
{
	struct LightingConfig
	{
		bool enableBlink = false;
		double blinkRateHz = 2.0;
		double maxIntensity = 1.0;
	};

	class LightingManager
	{
	private:
		std::unique_ptr<ILightingDriver> m_driver;
		LightingState m_currentState;
		LightingConfig m_config;

		double m_blinkTimer = 0.0;
		bool m_blinkState = true;

	public:
		void init()
		{
			// Read Config
			const bool isEnabled = ConfigIni::GetBool(ConfigIni::Key::kLightingEnable, true);
			if (!isEnabled)
			{
				m_driver = nullptr;
				return;
			}

			m_config.enableBlink = ConfigIni::GetBool(ConfigIni::Key::kLightingBlink, false);

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

			LightingState outputState = state;

			// Apply behaviors
			if (m_config.enableBlink)
			{
				m_blinkTimer += Scene::DeltaTime();
				if (m_blinkTimer > (1.0 / m_config.blinkRateHz))
				{
					m_blinkTimer = 0.0;
					m_blinkState = !m_blinkState;
				}

				if (!m_blinkState)
				{
					// Turn off blinking lights
					for (int i=0; i<4; ++i) outputState.bt[i] = false;
					for (int i=0; i<2; ++i) outputState.fx[i] = false;
				}
			}

			if (m_driver)
			{
				m_driver->update(outputState);
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

		void setConfig(const LightingConfig& config)
		{
			m_config = config;
		}
	};
}
