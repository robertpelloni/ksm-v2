#pragma once
#include "kson/common/common.hpp"

namespace kson
{
	struct EditorInfo
	{
		std::string appName;

		std::string appVersion;

		ByPulseMulti<std::string> comment;
	};
}
