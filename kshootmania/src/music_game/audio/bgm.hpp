#pragma once
#include "ksmaudio/ksmaudio.hpp"
#include "kson/audio/audio_effect.hpp"

namespace MusicGame::Audio
{
	struct SwitchAudioStream
	{
		std::string name;
		ksmaudio::StreamWithEffects stream;
		ksmaudio::AudioEffect::AudioEffectBus* pAudioEffectBusLaser;

		SwitchAudioStream(const std::string& name, const std::string& filePath, double volume, bool enableCompressor, bool preload)
			: name(name)
			, stream(filePath, volume, enableCompressor, preload)
			, pAudioEffectBusLaser(stream.emplaceAudioEffectBusLaser())
		{
		}

		SwitchAudioStream(const SwitchAudioStream&) = delete;
		SwitchAudioStream& operator=(const SwitchAudioStream&) = delete;
		SwitchAudioStream(SwitchAudioStream&&) = delete;
		SwitchAudioStream& operator=(SwitchAudioStream&&) = delete;
	};

	class BGM
	{
	private:
		ksmaudio::StreamWithEffects m_stream;
		const Duration m_duration;
		const SecondsF m_offset;
		SecondsF m_timeSec = 0s;
		bool m_isStreamStarted = false;
		bool m_isPaused = true;
		ksmaudio::AudioEffect::AudioEffectBus* const m_pAudioEffectBusFX;
		ksmaudio::AudioEffect::AudioEffectBus* const m_pAudioEffectBusLaser;
		Stopwatch m_stopwatch;
		Stopwatch m_manualUpdateStopwatch;

		// SwitchAudio音声のストリーム
		std::vector<std::unique_ptr<SwitchAudioStream>> m_switchAudioStreamsFX;
		std::vector<std::unique_ptr<SwitchAudioStream>> m_switchAudioStreamsLaser;
		Optional<std::size_t> m_activeSwitchAudioIdxFX;
		Optional<std::size_t> m_activeSwitchAudioIdxLaser;

		void emplaceAudioEffectImpl(
			bool isFX,
			const std::string& name,
			const kson::AudioEffectDef& def,
			const std::unordered_map<std::string, std::map<float, std::string>>& paramChanges,
			const std::set<float>& updateTriggerTiming);

		static void emplaceAudioEffectToBus(
			ksmaudio::AudioEffect::AudioEffectBus* pAudioEffectBus,
			const std::string& name,
			const kson::AudioEffectDef& def,
			const std::unordered_map<std::string, std::map<float, std::string>>& paramChanges,
			const std::set<float>& updateTriggerTiming);

	public:
		BGM(FilePathView filePath, double volume, SecondsF offset);

		void update();

		void updateAudioEffectFX(bool bypass, const ksmaudio::AudioEffect::Status& status, const ksmaudio::AudioEffect::ActiveAudioEffectDict& activeAudioEffects);

		// Siv3D非依存のksmaudioに渡す必要があるため、ここではOptionalではなくstd::optionalを使用(これらは暗黙変換されない)
		void updateAudioEffectLaser(bool bypass, const ksmaudio::AudioEffect::Status& status, const std::optional<std::size_t>& activeAudioEffectIdx);

		void play();

		void pause();

		void seekPosSec(SecondsF timeSec);

		SecondsF posSec() const;

		Duration duration() const;

		Duration latency() const;

		void emplaceAudioEffectFX(
			const std::string& name,
			const kson::AudioEffectDef& def,
			const std::unordered_map<std::string, std::map<float, std::string>>& paramChanges,
			const std::set<float>& updateTriggerTiming = {});

		void emplaceAudioEffectLaser(
			const std::string& name,
			const kson::AudioEffectDef& def,
			const std::unordered_map<std::string, std::map<float, std::string>>& paramChanges,
			const std::set<float>& updateTriggerTiming = {});

		const ksmaudio::AudioEffect::AudioEffectBus& audioEffectBusFX() const;

		const ksmaudio::AudioEffect::AudioEffectBus& audioEffectBusLaser() const;

		void setFadeOut(Duration duration);

		void emplaceSwitchAudioStream(
			bool isFX,
			const std::string& effectName,
			const std::string& filename,
			const FilePath& parentPath);

		void updateSwitchAudio(Optional<std::size_t> switchAudioIdxFX, Optional<std::size_t> switchAudioIdxLaser);

		Optional<std::size_t> switchAudioIdxByNameFX(const std::string& name) const;
		Optional<std::size_t> switchAudioIdxByNameLaser(const std::string& name) const;

		void emplaceSwitchAudioLaserEffect(
			const std::string& name,
			const kson::AudioEffectDef& def,
			const std::unordered_map<std::string, std::map<float, std::string>>& paramChanges,
			const std::set<float>& updateTriggerTiming = {});
	};
}
