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
			// Load controller profiles from JSON
			Array<ControllerProfile> profiles;
			const FilePath profilesPath = U"hardware/controller_profiles.json";

			if (FileSystem::Exists(profilesPath))
			{
				JSON json = JSON::Load(profilesPath);
				if (json && json.hasElement(U"profiles") && json[U"profiles"].isArray())
				{
					for (const auto& p : json[U"profiles"].arrayView())
					{
						ControllerProfile profile;
						profile.name = p[U"name"].getString();
						profile.vid = static_cast<uint16_t>(p[U"vid"].getOr<int32>(0));
						profile.pid = static_cast<uint16_t>(p[U"pid"].getOr<int32>(0));
						profile.reportId = static_cast<uint8_t>(p[U"report_id"].getOr<int32>(0));
						profile.reportLength = p[U"report_length"].getOr<int32>(4);
						profile.buttonsByte = p[U"buttons_byte"].getOr<int32>(1);
						profile.btABit = p[U"bt_a_bit"].getOr<int32>(0);
						profile.btBBit = p[U"bt_b_bit"].getOr<int32>(1);
						profile.btCBit = p[U"bt_c_bit"].getOr<int32>(2);
						profile.btDBit = p[U"bt_d_bit"].getOr<int32>(3);
						profile.fxLBit = p[U"fx_l_bit"].getOr<int32>(4);
						profile.fxRBit = p[U"fx_r_bit"].getOr<int32>(5);
						profile.laserLByte = p[U"laser_l_byte"].getOr<int32>(2);
						profile.laserRByte = p[U"laser_r_byte"].getOr<int32>(3);
						profiles.push_back(profile);
					}
				}
			}

			// If no profiles loaded, use a default YuanCon generic profile
			if (profiles.empty())
			{
				profiles.push_back(ControllerProfile{});
			}

			// Attempt to open the first matching connected controller
			bool foundDevice = false;
			for (const auto& profile : profiles)
			{
				auto hidDriver = std::make_unique<HidLightingDriver>(profile);
				if (hidDriver->init())
				{
					m_driver = std::move(hidDriver);
					foundDevice = true;
					break;
				}
			}

			if (!foundDevice)
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
