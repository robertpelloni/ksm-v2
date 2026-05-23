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
			// Read profiles
			Array<ControllerProfile> profiles;
			JSON json = JSON::Load(U"hardware/controller_profiles.json");
			if (json)
			{
				for (const auto& item : json.arrayView())
				{
					ControllerProfile p;
					p.name = item[U"name"].getString();
					p.vid = ParseInt<uint16_t>(item[U"vid"].getString().replace(U"0x", U""), Arg::radix = 16);
					p.pid = ParseInt<uint16_t>(item[U"pid"].getString().replace(U"0x", U""), Arg::radix = 16);
					p.reportId = ParseInt<uint8_t>(item[U"reportId"].getString().replace(U"0x", U""), Arg::radix = 16);
					p.reportLength = item[U"reportLength"].get<int32>();
					p.buttonsByte = item[U"buttonsByte"].get<int32>();
					p.btABit = item[U"btABit"].get<int32>();
					p.btBBit = item[U"btBBit"].get<int32>();
					p.btCBit = item[U"btCBit"].get<int32>();
					p.btDBit = item[U"btDBit"].get<int32>();
					p.fxLBit = item[U"fxLBit"].get<int32>();
					p.fxRBit = item[U"fxRBit"].get<int32>();
					p.laserLByte = item[U"laserLByte"].get<int32>();
					p.laserRByte = item[U"laserRByte"].get<int32>();
					profiles.push_back(p);
				}
			}

			if (profiles.empty())
			{
				// Fallback to Yuancon standard if file missing or empty
				profiles.push_back(ControllerProfile{});
			}

			bool connected = false;
			for (const auto& profile : profiles)
			{
				auto hidDriver = std::make_unique<HidLightingDriver>(profile);
				if (hidDriver->init())
				{
					m_driver = std::move(hidDriver);
					connected = true;
					break;
				}
			}

			if (!connected)
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
