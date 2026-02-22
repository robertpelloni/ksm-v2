#pragma once
#include <Siv3D.hpp>
#include "Hardware/Lighting/ILightingDriver.hpp"

namespace Debug
{
	class LightingOverlay
	{
	public:
		static void Draw(const Hardware::Lighting::LightingState& state)
		{
			// Draw small indicators in bottom right or somewhere
			// BT A, B, C, D (White)
			// FX L, R (Orange)
			// Laser L (Blue), R (Pink)

			const Vec2 basePos = Scene::Size() - Vec2(200, 50);

			// FX L
			RectF(basePos + Vec2(0, 20), 40, 20).draw(state.fx[0] ? Palette::Orange : Palette::Gray);
			// FX R
			RectF(basePos + Vec2(160, 20), 40, 20).draw(state.fx[1] ? Palette::Orange : Palette::Gray);

			// BT A B C D
			RectF(basePos + Vec2(20, 0), 30, 30).draw(state.bt[0] ? Palette::White : Palette::Gray);
			RectF(basePos + Vec2(60, 0), 30, 30).draw(state.bt[1] ? Palette::White : Palette::Gray);
			RectF(basePos + Vec2(100, 0), 30, 30).draw(state.bt[2] ? Palette::White : Palette::Gray);
			RectF(basePos + Vec2(140, 0), 30, 30).draw(state.bt[3] ? Palette::White : Palette::Gray);

			// Laser indicators (Bar height)
			// L
			RectF(basePos + Vec2(0, 0), 10, 40).draw(state.laser[0]);
			// R
			RectF(basePos + Vec2(190, 0), 10, 40).draw(state.laser[1]);
		}
	};
}
