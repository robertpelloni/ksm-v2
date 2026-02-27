#include "ksmaudio/ksmaudio.hpp"
#include "bass.h"

namespace ksmaudio
{
	namespace
	{
		double s_masterVolume = 1.0;
		bool s_isMuted = false;

		void ApplyVolume()
		{
			const DWORD bassVolume = s_isMuted ? 0 : static_cast<DWORD>(s_masterVolume * 10000.0);
			BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, bassVolume);
			BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, bassVolume);
		}
	}

	std::vector<AudioDeviceInfo> GetAudioDevices()
	{
		std::vector<AudioDeviceInfo> devices;
		BASS_DEVICEINFO info;
		// Device 0 is "No Sound", 1 is first real device.
		// BASS_GetDeviceInfo iterates from 1.
		for (int i = 1; BASS_GetDeviceInfo(i, &info); i++)
		{
			if (info.flags & BASS_DEVICE_ENABLED)
			{
				devices.push_back({
					i,
					info.name ? info.name : "Unknown",
					info.driver ? info.driver : "",
					(info.flags & BASS_DEVICE_DEFAULT) != 0
				});
			}
		}
		return devices;
	}

	void Init(void* hWnd, int deviceId, DWORD sampleRate, DWORD bufferMs, DWORD updatePeriodMs)
	{
		// Force default device if -1, but usually user passes specific ID or -1.
		// BASS_Init(device, ...)

#ifdef _WIN32
		if (!BASS_Init(deviceId, sampleRate, 0, static_cast<HWND>(hWnd), nullptr))
#else
		(void)hWnd;
		if (!BASS_Init(deviceId, sampleRate, 0, 0, nullptr))
#endif
		{
			// Fallback to default device if specific failed?
			if (deviceId != -1)
			{
				// Try default
#ifdef _WIN32
				BASS_Init(-1, sampleRate, 0, static_cast<HWND>(hWnd), nullptr);
#else
				BASS_Init(-1, sampleRate, 0, 0, nullptr);
#endif
			}
		}

		BASS_SetConfig(BASS_CONFIG_BUFFER, bufferMs);
		BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, updatePeriodMs);
		BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);
		BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, kUpdateThreads);

		BASS_FX_GetVersion(); // bass_fx.dllをロードするために呼ぶ必要あり
	}

	void Terminate()
	{
		BASS_Free();
	}

	void SetMute(bool isMute)
	{
		s_isMuted = isMute;
		ApplyVolume();
	}

	void SetMasterVolume(double volume)
	{
		s_masterVolume = volume;
		ApplyVolume();
	}
}
