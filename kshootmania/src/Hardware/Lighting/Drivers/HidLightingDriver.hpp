#pragma once
#include "ILightingDriver.hpp"

#if defined(SIV3D_PLATFORM_LINUX) || defined(SIV3D_PLATFORM_MACOS)
#include <hidapi/hidapi.h>
#elif defined(SIV3D_PLATFORM_WINDOWS)
#include <hidapi.h> // Windows usually has it in include path if set up
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

		hid_device* m_device = nullptr;
		uint16_t m_vid = 0;
		uint16_t m_pid = 0;

	public:
		HidLightingDriver(uint16_t vid, uint16_t pid) : m_vid(vid), m_pid(pid) {}

		bool init() override
		{
#ifdef SIV3D_PLATFORM_WINDOWS
			// Windows HIDAPI init if needed
#endif
			if (hid_init() != 0)
			{
				Logger << U"[ksm error] hid_init failed";
				return false;
			}

			// Try to open
			m_device = hid_open(m_vid, m_pid, nullptr);
			if (!m_device)
			{
				// Logger << U"[ksm warning] Failed to open HID device {:04x}:{:04x}"_fmt(m_vid, m_pid);
				return false;
			}

			Logger << U"[ksm info] HID device opened: {:04x}:{:04x}"_fmt(m_vid, m_pid);
			return true;
		}

		void update(const LightingState& state) override
		{
			if (!m_device) return;

			// Construct HID report
			// This depends heavily on the controller firmware.
			// YuanCon standard:
			// Report ID?
			// Byte 0: Lights (Bitmask)
			// Bit 0-3: BT A-D
			// Bit 4-5: FX L/R
			// Byte 1: Laser L (Blue?)
			// Byte 2: Laser R (Pink?)

			// This is a placeholder protocol. We need a way to configure this.
			// For this task, I'll implement a "Standard" report format often used.

			uint8_t report[64] = {0};

			// Simple Bitmask for buttons
			uint8_t buttons = 0;
			if (state.bt[0]) buttons |= (1 << 0);
			if (state.bt[1]) buttons |= (1 << 1);
			if (state.bt[2]) buttons |= (1 << 2);
			if (state.bt[3]) buttons |= (1 << 3);
			if (state.fx[0]) buttons |= (1 << 4);
			if (state.fx[1]) buttons |= (1 << 5);

			report[0] = 0; // Report ID if needed, or data
			// Wait, hid_write expects Report ID as first byte if numbered reports are used.
			// If not, it's just data.

			// Assuming YuanCon style (often just sends state)
			// Let's assume a generic report structure for now.
			// Byte 0: Buttons
			// Byte 1: Laser L Brightness
			// Byte 2: Laser R Brightness

			uint8_t buffer[65];
			buffer[0] = 0x00; // Report ID
			buffer[1] = buttons;
			buffer[2] = static_cast<uint8_t>(state.laser[0].r); // Brightness/Color? Usually just intensity 0-255
			buffer[3] = static_cast<uint8_t>(state.laser[1].r); // Using Red channel as intensity for now

			hid_write(m_device, buffer, 4);
		}

		void close() override
		{
			if (m_device)
			{
				hid_close(m_device);
				m_device = nullptr;
			}
			hid_exit();
		}
	};
}
