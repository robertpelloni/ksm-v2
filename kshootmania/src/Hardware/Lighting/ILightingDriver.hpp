#pragma once
#include <Siv3D.hpp>

namespace Hardware::Lighting
{
	struct LightingState
	{
		bool bt[4] = { false, false, false, false };
		bool fx[2] = { false, false };
		Color laser[2] = { Color(0,0,0), Color(0,0,0) }; // Off if black
	};

	class ILightingDriver
	{
	public:
		virtual ~ILightingDriver() = default;
		virtual bool init() = 0;
		virtual void update(const LightingState& state) = 0;
		virtual void close() = 0;
	};
}
