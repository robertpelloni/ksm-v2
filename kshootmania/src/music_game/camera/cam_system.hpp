#pragma once
#include "kson/chart_data.hpp"

namespace MusicGame::Camera
{
	struct CamStatus
	{
		double zoomBottom = 0.0;
		double shiftX = 0.0;
		double zoomTop = 0.0;
		bool useLegacyZoomTop = false;
		double rotationZ = 0.0;
		double rotationZHighway = 0.0;
		double rotationZJdgline = 0.0;
		double rotationZLayer = 0.0;
		double centerSplit = 0.0;
	};

	class CamSystem
	{
	private:
		CamStatus m_status;

		// KSHバージョン167未満での視点変更の緩和時間適用
		bool m_useLegacyZoomRelaxation = false;
		double m_legacyCurrentZoomTop = 0.0;
		double m_legacyCurrentZoomBottom = 0.0;
		double m_legacyCurrentZoomSide = 0.0;

	public:
		explicit CamSystem(const kson::ChartData& chartData);

		void update(const kson::ChartData& chartData, kson::Pulse currentPulse);

		const CamStatus& status() const
		{
			return m_status;
		}
	};
}
