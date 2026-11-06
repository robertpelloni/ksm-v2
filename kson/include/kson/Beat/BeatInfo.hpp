#pragma once
#include "kson/common/common.hpp"
#include "TimeSig.hpp"

namespace kson
{
	struct BeatInfo
	{
		ByPulse<double> bpm;

		ByMeasureIdx<TimeSig> timeSig;

		Graph scrollSpeed;

		ByPulse<RelPulse> stop;
	};
}
