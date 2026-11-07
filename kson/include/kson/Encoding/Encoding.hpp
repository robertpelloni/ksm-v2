#pragma once
#include "kson/common/common.hpp"

namespace kson
{
	namespace Encoding
	{
		[[nodiscard]]
		std::string ShiftJISToUTF8(std::string_view shiftJISStr);
	}
}
