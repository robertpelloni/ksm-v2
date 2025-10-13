#include "fx_chip_se.hpp"
#include "audio_defines.hpp"

namespace MusicGame::Audio
{
	namespace
	{
		bool IsBuiltInSoundName(const std::string& filename)
		{
			return filename == "clap" ||
				filename == "clap_impact" ||
				filename == "clap_punchy" ||
				filename == "snare" ||
				filename == "snare_lo";
		}

		String GetFilePath(FilePathView parentPath, const std::string& filename)
		{
			if (IsBuiltInSoundName(filename))
			{
				// ビルトインの効果音
				return U"se/note/{}.wav"_fmt(Unicode::FromUTF8(filename));
			}
			else
			{
				// 譜面と同じフォルダの音声ファイル
				return FileSystem::PathAppend(parentPath, Unicode::FromUTF8(filename));
			}
		}
	}

	FXChipSE::FXChipSE(const kson::ChartData& chartData, FilePathView parentPath)
	{
		// キー音ファイルを読み込み
		const auto& chipEvent = chartData.audio.keySound.fx.chipEvent;
		for (const auto& [filename, lanes] : chipEvent)
		{
			if (m_keySounds.contains(filename))
			{
				continue;
			}

			const FilePath filePath = GetFilePath(parentPath, filename);
			if (!FileSystem::Exists(filePath))
			{
				Logger << U"[ksm warning] failed to open key sound '{}' (filePath:'{}')"_fmt(Unicode::FromUTF8(filename), filePath);
				continue;
			}

			// 同一効果音の同時再生数は1まで
			// TODO: 旧バージョンのkshでは同時再生数が異なる
			constexpr DWORD kMaxPolyphony = 1;
			m_keySounds.emplace(filename, ksmaudio::Sample{ filePath.narrow(), kMaxPolyphony });
		}
	}

	void FXChipSE::update(const kson::ChartData& chartData, const GameStatus& gameStatus)
	{
		const auto& chipEvent = chartData.audio.keySound.fx.chipEvent;

		for (std::size_t laneIdx = 0U; laneIdx < kson::kNumFXLanesSZ; ++laneIdx)
		{
			const auto& laneStatus = gameStatus.fxLaneStatus[laneIdx];
			const kson::Pulse chipPulse = laneStatus.lastJudgedChipPulse;
			const double judgmentTimeSec = laneStatus.lastChipJudgedTimeSec;

			if (m_lastPlayedTimeSecs[laneIdx] >= judgmentTimeSec)
			{
				// 最後に判定したチップの効果音が既に再生済みの場合は何もしない
				continue;
			}

			if (judgmentTimeSec > gameStatus.currentTimeSec + kSELatencySec)
			{
				// 最後に判定したチップのタイミングがまだ先にある場合は何もしない
				continue;
			}

			// このチップに対応するキー音を検索
			for (const auto& [filename, lanes] : chipEvent)
			{
				if (laneIdx >= lanes.size())
				{
					continue;
				}

				const auto it = lanes[laneIdx].find(chipPulse);
				if (it == lanes[laneIdx].end())
				{
					continue;
				}

				if (!m_keySounds.contains(filename))
				{
					Logger << U"[ksm warning] key sound not found: '{}'"_fmt(Unicode::FromUTF8(filename));
					continue;
				}

				// キー音を再生
				const double volume = it->second.vol;
				m_keySounds.at(filename).play(volume);
				m_lastPlayedTimeSecs[laneIdx] = judgmentTimeSec;
				break;
			}
		}
	}
}
