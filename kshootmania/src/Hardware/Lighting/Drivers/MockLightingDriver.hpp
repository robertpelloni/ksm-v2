#pragma once
#include "ILightingDriver.hpp"

namespace Hardware::Lighting
{
	class MockLightingDriver : public ILightingDriver
	{
	public:
		bool init() override
		{
			Logger << U"[ksm info] MockLightingDriver initialized.";
			return true;
		}

		void update(const LightingState& state) override
		{
			// Optional: Detailed logging or just keep silent to avoid spam
			// Logger << U"BT: {}{}{}{} FX: {}{}"_fmt(state.bt[0], state.bt[1], state.bt[2], state.bt[3], state.fx[0], state.fx[1]);
		}

		void close() override
		{
			Logger << U"[ksm info] MockLightingDriver closed.";
		}
	};
}
