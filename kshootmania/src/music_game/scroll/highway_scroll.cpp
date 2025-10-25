#include "highway_scroll.hpp"

namespace MusicGame::Scroll
{
	namespace
	{
		// ハイスピードの基準ピクセル数
		// HSP版:
		//     https://github.com/kshootmania/ksm-v1/blob/08275836547c7792a6d4f59037e56e947f2979c3/src/scene/play/play_key_input.hsp#L429-L440
		//     https://github.com/kshootmania/ksm-v1/blob/d2811a09e2d75dad5cc152d7c4073897061addb7/src/scene/play/play_draw_frame.hsp#L96
		// 上記の「108/2」と「10」を乗算した値にあたる
		constexpr double kBasePixels = 540.0;

		/// @brief scrollSpeedに負の値が含まれているかを判定
		/// @param beatInfo kson.beat
		/// @return 負の値が含まれていればtrue
		bool HasNegativeScrollSpeed(const kson::BeatInfo& beatInfo)
		{
			// scrollSpeedグラフに負の値が1つでも含まれていればtrueを返す
			for (const auto& [pulse, graphValue] : beatInfo.scrollSpeed)
			{
				if (graphValue.v.v < 0.0 || graphValue.v.vf < 0.0)
				{
					return true;
				}
			}
			return false;
		}

		/// @brief BPMの最頻値(累計Pulse値が最も大きいBPM)を返す
		/// @param chartData 譜面データ
		/// @return BPM
		/// @remarks BPM変更が2個以上ある場合、小数部分は無視する(そのため、BPMが整数でない場合にハイスピード値に若干の誤差が出るが、仕様)
		double GetModeBPM(const kson::ChartData& chartData)
		{
			constexpr double kErrorBPM = 120.0;

			if (chartData.beat.bpm.empty())
			{
				// BPMは1個以上存在するはず
				assert(false && "kson.beat.bpm is empty");
				return kErrorBPM;
			}

			if (chartData.beat.bpm.size() == 1U)
			{
				// BPMが1個の場合はその値を返せばOK
				return chartData.beat.bpm.begin()->second;
			}

			// 各BPM値が占めるPulse値の合計を調べる
			std::unordered_map<int32, kson::RelPulse> bpmTotalPulses;
			kson::Pulse prevY = kson::Pulse{ 0 };
			Optional<int32> prevBPMInt = none;
			for (const auto& [y, bpm] : chartData.beat.bpm)
			{
				if (y < prevY)
				{
					assert(false && "y must be larger than or equal to prevY in kson.beat.bpm");
					return kErrorBPM;
				}
				if (prevBPMInt.has_value())
				{
					// 存在しない場合はoperator[]でゼロ初期化されるので明示的な代入不要
					bpmTotalPulses[prevBPMInt.value()] += y - prevY;
				}

				prevY = y;
				prevBPMInt = static_cast<int32>(bpm);
			}

			// 最終BPM変化を加味する
			// (最終ノーツのPulse値を調べ、最終BPM変化より後ろであれば加味)
			const kson::Pulse lastNoteEndY = kson::LastNoteEndY(chartData.note);
			if (lastNoteEndY > prevY)
			{
				if (!prevBPMInt.has_value())
				{
					assert(false && "prevBPMInt must not be none after the loop");
					return kErrorBPM;
				}
				bpmTotalPulses[prevBPMInt.value()] += lastNoteEndY - prevY;
			}

			if (bpmTotalPulses.empty())
			{
				assert(false && "bpmPulses must not be empty");
				return kErrorBPM;
			}

			// 最も累計Pulse値が大きいBPMを調べる
			const auto itr = std::max_element(
				bpmTotalPulses.begin(),
				bpmTotalPulses.end(),
				[](const auto& a, const auto& b) { return a.second < b.second; });

			if (itr == bpmTotalPulses.end())
			{
				assert(false && "max_element must not return end iterator because bpmPulses must not be empty");
				return kErrorBPM;
			}

			const auto& [modeBPM, modeBPMTotalPulse] = *itr;
			return static_cast<double>(modeBPM);
		}

		/// @brief ハイスピード係数を求める
		/// @param hispeedSetting ハイスピード設定
		/// @param stdBPM 基準BPM
		/// @return ハイスピード係数
		double HispeedFactor(const HispeedSetting& hispeedSetting, double stdBPM)
		{
			switch (hispeedSetting.type)
			{
			case HispeedType::XMod:
				return static_cast<double>(hispeedSetting.value) / 10;

			case HispeedType::OMod:
				return static_cast<double>(hispeedSetting.value) / stdBPM;

			case HispeedType::CMod:
				return static_cast<double>(hispeedSetting.value);

			default:
				assert(false && "Unknown hispeed type");
				return static_cast<double>(hispeedSetting.value);
			}
		}

		/// @brief ハイスピード値を求める
		/// @param hispeedSetting ハイスピード設定
		/// @param currentBPM 現在のBPM
		/// @param stdBPM 基準BPM
		/// @return ハイスピード係数
		/// @note HispeedFactorにBPMをかけた値と基本的に同じだが、C-modで設定値をそのまま返す点、整数のまま計算する点が異なる
		int32 CurrentHispeed(const HispeedSetting& hispeedSetting, double currentBPM, double stdBPM)
		{
			switch (hispeedSetting.type)
			{
			case HispeedType::XMod:
				return static_cast<int32>(currentBPM) * hispeedSetting.value / 10;

			case HispeedType::OMod:
				return static_cast<int32>(currentBPM * hispeedSetting.value / stdBPM);

			case HispeedType::CMod:
				return hispeedSetting.value;

			default:
				assert(false && "Unknown hispeed type");
				return static_cast<int32>(currentBPM);
			}
		}

		/// @brief scrollSpeedを考慮したノーツの相対Pulse値を計算
		/// @param notePulse ノーツのPulse位置
		/// @param currentPulseDouble 現在のPulse位置
		/// @param scrollSpeed scrollSpeedグラフ
		/// @return scrollSpeedを考慮した相対Pulse値
		double CalcScrollSpeedAdjustedRelPulse(kson::Pulse notePulse, double currentPulseDouble, const kson::Graph& scrollSpeed)
		{
			if (scrollSpeed.empty())
			{
				return static_cast<double>(notePulse - currentPulseDouble);
			}

			const kson::Pulse currentPulse = static_cast<kson::Pulse>(currentPulseDouble);

			// notePulseが現在位置より未来の場合
			if (notePulse > currentPulse)
			{
				double totalRelPulse = 0.0;
				kson::Pulse segmentStartPulse = currentPulse;
				double currentSpeed = kson::GraphValueAt(scrollSpeed, segmentStartPulse);

				while (segmentStartPulse < notePulse)
				{
					auto nextItr = scrollSpeed.upper_bound(segmentStartPulse);

					kson::Pulse segmentEndPulse;
					double nextSpeed;
					if (nextItr != scrollSpeed.end() && nextItr->first <= notePulse)
					{
						segmentEndPulse = nextItr->first;
						nextSpeed = nextItr->second.v.v;
					}
					else
					{
						segmentEndPulse = notePulse;
						nextSpeed = currentSpeed;
					}

					// 台形則で積分
					const kson::Pulse length = segmentEndPulse - segmentStartPulse;
					totalRelPulse += length * (currentSpeed + nextSpeed) / 2.0;

					// 次の区間の開始速度を設定
					if (nextItr != scrollSpeed.end() && nextItr->first == segmentEndPulse)
					{
						currentSpeed = nextItr->second.v.vf;
					}
					else
					{
						currentSpeed = nextSpeed;
					}

					segmentStartPulse = segmentEndPulse;
				}

				return totalRelPulse;
			}
			// notePulseが現在位置より過去の場合
			else
			{
				double totalRelPulse = 0.0;
				kson::Pulse segmentEndPulse = currentPulse;
				double endSpeed = kson::GraphValueAt(scrollSpeed, segmentEndPulse);

				while (segmentEndPulse > notePulse)
				{
					// segmentEndPulse未満で最大のscroll_speed変更点を探す
					auto itr = scrollSpeed.lower_bound(segmentEndPulse);
					if (itr != scrollSpeed.begin())
					{
						--itr;
					}

					kson::Pulse segmentStartPulse;
					double startSpeed;
					if (itr != scrollSpeed.end() && itr->first > notePulse)
					{
						segmentStartPulse = itr->first;
						// 区間の開始点での速度はv.vfではなくv.v
						startSpeed = itr->second.v.v;
					}
					else
					{
						segmentStartPulse = notePulse;
						startSpeed = endSpeed;
					}

					// 台形則で積分(負の方向)
					const kson::Pulse length = segmentEndPulse - segmentStartPulse;
					totalRelPulse -= length * (startSpeed + endSpeed) / 2.0;

					// 次の区間へ移動
					// 次の区間の終了速度を設定(scroll_speed変更点がある場合はその直前の速度)
					if (itr != scrollSpeed.end() && itr->first == segmentStartPulse)
					{
						// itrがbeginの場合、これ以上前に戻れないのでループ終了
						if (itr == scrollSpeed.begin())
						{
							break;
						}
						// 一つ前のscroll_speed変更点の終了速度を取得
						auto prevItr = std::prev(itr);
						endSpeed = prevItr->second.v.vf;
					}
					else
					{
						endSpeed = startSpeed;
					}

					segmentEndPulse = segmentStartPulse;
				}

				return totalRelPulse;
			}
		}
	}

	HighwayScrollContext::HighwayScrollContext(const HighwayScroll* pHighwayScroll, const kson::BeatInfo* pBeatInfo, const kson::TimingCache* pTimingCache, const GameStatus* pGameStatus)
		: m_pHighwayScroll(pHighwayScroll)
		, m_pBeatInfo(pBeatInfo)
		, m_pTimingCache(pTimingCache)
		, m_pGameStatus(pGameStatus)
		, m_hasNegativeScrollSpeed(HasNegativeScrollSpeed(*pBeatInfo))
	{
	}

	HighwayScrollContext::~HighwayScrollContext() = default;

	int32 HighwayScrollContext::getPositionY(kson::Pulse pulse) const
	{
		return m_pHighwayScroll->getPositionY(pulse, *m_pBeatInfo, *m_pTimingCache, *m_pGameStatus);
	}

	const HighwayScroll& HighwayScrollContext::highwayScroll() const
	{
		return *m_pHighwayScroll;
	}

	bool HighwayScrollContext::hasNegativeScrollSpeed() const
	{
		return m_hasNegativeScrollSpeed;
	}

	bool HighwayScrollContext::isScrollSpeedPositiveAt(kson::Pulse pulse) const
	{
		// 負のscroll_speedが存在しない場合は常に正
		if (!m_hasNegativeScrollSpeed)
		{
			return true;
		}

		return kson::GraphValueAt(m_pBeatInfo->scrollSpeed, pulse) >= 0.0;
	}

	double HighwayScroll::pulseToSec(kson::Pulse pulse, const kson::BeatInfo& beatInfo, const kson::TimingCache& timingCache) const
	{
		// mutableな変数を操作するのでロック
		std::lock_guard lock(m_mutex);

		// 既に結果がキャッシュ上にあればキャッシュの値を返す
		if (m_pulseToSecCache.contains(pulse))
		{
			return m_pulseToSecCache.at(pulse);
		}

		// キャッシュ上になければ計算してキャッシュする
		const double sec = kson::PulseToSec(pulse, beatInfo, timingCache);
		m_pulseToSecCache.emplace(pulse, sec);
		return sec;
	}

	double HighwayScroll::getRelPulseEquvalent(kson::Pulse pulse, const kson::BeatInfo& beatInfo, const kson::TimingCache& timingCache, const GameStatus& gameStatus) const
	{
		if (m_hispeedSetting.type == HispeedType::CMod)
		{
			const double sec = pulseToSec(pulse, beatInfo, timingCache);
			const double relTimeSec = sec - gameStatus.currentTimeSec;
			return relTimeSec / 60 * kson::kResolution;
		}
		else
		{
			// scrollSpeedがなければ単純な差分計算
			if (beatInfo.scrollSpeed.empty())
			{
				return static_cast<double>(pulse) - gameStatus.currentPulseDouble;
			}

			// scrollSpeedがあれば現在地点からノーツ地点までの区間を積分計算
			return CalcScrollSpeedAdjustedRelPulse(pulse, gameStatus.currentPulseDouble, beatInfo.scrollSpeed);
		}
	}

	HighwayScroll::HighwayScroll(const kson::ChartData& chartData)
		: m_stdBPM(chartData.meta.stdBPM > 0.0 ? chartData.meta.stdBPM : GetModeBPM(chartData))
	{
	}

	void HighwayScroll::update(const HispeedSetting& hispeedSetting, double currentBPM)
	{
		m_hispeedSetting = hispeedSetting;
		m_currentBPM = currentBPM;
		m_hispeedFactor = HispeedFactor(hispeedSetting, m_stdBPM);
		m_currentHispeed = CurrentHispeed(hispeedSetting, currentBPM, m_stdBPM);
	}

	int32 HighwayScroll::getPositionY(kson::Pulse pulse, const kson::BeatInfo& beatInfo, const kson::TimingCache& timingCache, const GameStatus& gameStatus) const
	{
		assert(m_hispeedFactor != 0.0 && "HighwayScroll::update() must be called at least once before HighwayScroll::getPositionY()");

		const double relPulseEquivalent = getRelPulseEquvalent(pulse, beatInfo, timingCache, gameStatus);
		return Graphics::kHighwayTextureSize.y - static_cast<int32>(relPulseEquivalent * kBasePixels * m_hispeedFactor / kson::kResolution4);
	}
	
	const HispeedSetting& HighwayScroll::hispeedSetting() const
	{
		return m_hispeedSetting;
	}

	int32 HighwayScroll::nearestHispeedSettingValue(HispeedType targetHispeedType) const
	{
		switch (targetHispeedType)
		{
		case HispeedType::XMod:
			return MathUtils::RoundToInt(m_currentHispeed * 10 / Max(m_currentBPM, 1.0));

		case HispeedType::OMod:
			return MathUtils::RoundToInt(m_currentHispeed * m_stdBPM / Max(m_currentBPM, 1.0));

		case HispeedType::CMod:
			return m_currentHispeed;

		default:
			assert(false && "Unknown hispeed type");
			return 0;
		}
	}

	int32 HighwayScroll::currentHispeed() const
	{
		return m_currentHispeed;
	}
}
