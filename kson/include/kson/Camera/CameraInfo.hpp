#pragma once
#include "kson/common/common.hpp"
#include "Cam.hpp"
#include "Tilt.hpp"

namespace kson
{
	struct CameraInfo
	{
		CamInfo cam;
		ByPulse<TiltValue> tilt;
	};
}
