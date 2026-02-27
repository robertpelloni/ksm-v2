#pragma once
#include "Stream.hpp"
#include "StreamWithEffects.hpp"
#include "Sample.hpp"
#include "AudioEffect/All.hpp"
#include <string>
#include <vector>

namespace ksmaudio
{
	constexpr DWORD kDefaultSampleRate = 44100;
	constexpr DWORD kDefaultBufferSizeMs = 100; // Lower latency default? (200 was high)
	constexpr DWORD kDefaultUpdatePeriodMs = 10; // More frequent updates? (100 was high)
	constexpr DWORD kUpdateThreads = 2;

	struct AudioDeviceInfo
	{
		int id;
		std::string name;
		std::string driver;
		bool isDefault;
	};

	std::vector<AudioDeviceInfo> GetAudioDevices();

	void Init(void* hWnd, int deviceId = -1, DWORD sampleRate = kDefaultSampleRate, DWORD bufferMs = kDefaultBufferSizeMs, DWORD updatePeriodMs = kDefaultUpdatePeriodMs);

	void Terminate();

	void SetMute(bool isMute);

	/// @brief マスターボリュームを設定
	/// @param volume 音量(0.0〜1.0)
	void SetMasterVolume(double volume);
}
