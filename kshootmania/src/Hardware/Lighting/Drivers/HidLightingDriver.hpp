#pragma once
#include "ILightingDriver.hpp"
#include "../ControllerProfile.hpp"

#ifdef KSM_HIDAPI_ENABLED
#include <hidapi/hidapi.h> // Or <hidapi.h> depending on platform/include path
#endif

namespace Hardware::Lighting
{
	class HidLightingDriver : public ILightingDriver
	{
	private:
		// Common rhythm game controller VID/PID (e.g. YuanCon, SVSE)
		// For now, we might want to make this configurable or scan multiple.
		// Standard YuanCon: 0x1973 / 0x2001 (Example)
		// Let's use a generic approach or config-based.

		void* m_device = nullptr; // Use void* to avoid hidapi header dependency in headers if not strictly needed, but it's fine.
		ControllerProfile m_profile;

	public:
		HidLightingDriver(const ControllerProfile& profile) : m_profile(profile) {}

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

			Logger << U"[ksm info] HID device opened: {:04x}:{:04x} ({})"_fmt(m_profile.vid, m_profile.pid, m_profile.name);
			return true;
#else
			return false;
#endif
		}

					void update(const LightingState& state) override
			{
#ifdef KSM_HIDAPI_ENABLED
				if (!m_device) return;

				Array<uint8_t> buffer;
				buffer.resize(m_profile.reportLength + 1, 0x00);

				uint8_t buttons = 0;
				if (state.bt[0]) buttons |= (1 << m_profile.btABit);
				if (state.bt[1]) buttons |= (1 << m_profile.btBBit);
				if (state.bt[2]) buttons |= (1 << m_profile.btCBit);
				if (state.bt[3]) buttons |= (1 << m_profile.btDBit);
				if (state.fx[0]) buttons |= (1 << m_profile.fxLBit);
				if (state.fx[1]) buttons |= (1 << m_profile.fxRBit);

				buffer[0] = m_profile.reportId;

				if (m_profile.buttonsByte + 1 < buffer.size())
				{
					buffer[m_profile.buttonsByte + 1] = buttons;
				}

				if (m_profile.laserLByte + 1 < buffer.size())
				{
					buffer[m_profile.laserLByte + 1] = static_cast<uint8_t>(state.laser[0].r);
				}

				if (m_profile.laserRByte + 1 < buffer.size())
				{
					buffer[m_profile.laserRByte + 1] = static_cast<uint8_t>(state.laser[1].r);
				}

				hid_write(static_cast<hid_device*>(m_device), buffer.data(), buffer.size());
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
