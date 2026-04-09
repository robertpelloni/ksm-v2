#pragma once
#include "ILightingDriver.hpp"

#ifdef KSM_HIDAPI_ENABLED
#include <hidapi/hidapi.h> // Or <hidapi.h> depending on platform/include path
#endif
#include "../ControllerProfile.hpp"

namespace Hardware::Lighting
{
	class HidLightingDriver : public ILightingDriver
	{
	private:
		void* m_device = nullptr; // Use void* to avoid hidapi header dependency in headers if not strictly needed, but it's fine.
		ControllerProfile m_profile;

	public:
		explicit HidLightingDriver(const ControllerProfile& profile) : m_profile(profile) {}

		bool init() override
		{
#ifdef KSM_HIDAPI_ENABLED
			if (hid_init() != 0)
			{
				Logger << U"[ksm error] hid_init failed";
				return false;
			}

			// Try to open
			m_device = hid_open(m_profile.vid, m_profile.pid, nullptr);
			if (!m_device)
			{
				return false;
			}

			Logger << U"[ksm info] HID device opened for Lighting: {} ({:04x}:{:04x})"_fmt(m_profile.name, m_profile.vid, m_profile.pid);
			return true;
#else
			return false;
#endif
		}

		void update(const LightingState& state) override
		{
#ifdef KSM_HIDAPI_ENABLED
			if (!m_device) return;

			uint8_t buffer[65] = {0}; // Max typical HID report size + 1
			buffer[0] = m_profile.reportId;

			// Set buttons bitmask
			uint8_t buttons = 0;
			if (state.bt[0]) buttons |= (1 << m_profile.btABit);
			if (state.bt[1]) buttons |= (1 << m_profile.btBBit);
			if (state.bt[2]) buttons |= (1 << m_profile.btCBit);
			if (state.bt[3]) buttons |= (1 << m_profile.btDBit);
			if (state.fx[0]) buttons |= (1 << m_profile.fxLBit);
			if (state.fx[1]) buttons |= (1 << m_profile.fxRBit);

			if (m_profile.buttonsByte >= 0 && m_profile.buttonsByte < 64)
			{
				buffer[m_profile.buttonsByte] = buttons;
			}

			// Set lasers
			if (m_profile.laserLByte >= 0 && m_profile.laserLByte < 64)
			{
				buffer[m_profile.laserLByte] = static_cast<uint8_t>(state.laser[0].r); // Simple intensity
			}
			if (m_profile.laserRByte >= 0 && m_profile.laserRByte < 64)
			{
				buffer[m_profile.laserRByte] = static_cast<uint8_t>(state.laser[1].r);
			}

			hid_write(static_cast<hid_device*>(m_device), buffer, m_profile.reportLength);
#endif
		}

		void close() override
		{
#ifdef KSM_HIDAPI_ENABLED
			if (m_device)
			{
				hid_close(static_cast<hid_device*>(m_device));
				m_device = nullptr;
			}
			hid_exit();
#endif
		}
	};
}
