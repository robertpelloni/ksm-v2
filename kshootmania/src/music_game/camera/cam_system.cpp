#include "cam_system.hpp"
#include "kson/util/graph_utils.hpp"

namespace MusicGame::Camera
{
	void CamSystem::update(const kson::ChartData& chartData, kson::Pulse currentPulse)
	{
		const auto& body = chartData.camera.cam.body;
		m_status.zoomBottom = kson::GraphValueAt(body.zoomBottom, currentPulse);
		m_status.shiftX = kson::GraphValueAt(body.zoomSide, currentPulse) / 3.0;
		m_status.zoomTop = kson::GraphValueAt(body.zoomTop, currentPulse);
		m_status.rotationZ = kson::GraphValueAt(body.rotationZ, currentPulse);
		m_status.centerSplit = kson::GraphValueAt(body.centerSplit, currentPulse);
	}
}
