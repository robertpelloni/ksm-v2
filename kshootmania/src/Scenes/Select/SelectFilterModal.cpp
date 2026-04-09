#include "SelectFilterModal.hpp"
#include "Input/KeyConfig.hpp"
#include "Common/AssetManagement.hpp"
#include "Common/CommonDefines.hpp"

void SelectFilterModal::update()
{
	if (!m_isActive) return;

	// In a real implementation we would have an interactive menu
	// For now, toggle sort type with Up/Down, and Level Filter with Left/Right

	if (KeyConfig::Down(KeyConfig::kButtonUp))
	{
		m_sortTypeIdx = (m_sortTypeIdx - 1 + static_cast<int32>(m_sortTypes.size())) % static_cast<int32>(m_sortTypes.size());
	}
	else if (KeyConfig::Down(KeyConfig::kButtonDown))
	{
		m_sortTypeIdx = (m_sortTypeIdx + 1) % static_cast<int32>(m_sortTypes.size());
	}

	if (KeyConfig::Down(KeyConfig::kButtonRight))
	{
		m_levelFilter++;
		if (m_levelFilter > kLevelMax) m_levelFilter = 0; // Wrap around to ALL
	}
	else if (KeyConfig::Down(KeyConfig::kButtonLeft))
	{
		m_levelFilter--;
		if (m_levelFilter < 0) m_levelFilter = kLevelMax;
	}

	// Close on start or back
	if (KeyConfig::Down(KeyConfig::kButtonStart) || KeyConfig::Down(KeyConfig::kButtonBack))
	{
		toggle();
	}
}

void SelectFilterModal::draw() const
{
	if (!m_isActive) return;

	Scene::Rect().draw(ColorF(0.0, 0.7));

	const RectF box(Arg::center = Scene::Center(), 400, 300);
	box.draw(ColorF(0.1, 0.9)).drawFrame(2, Palette::Cyan);

	const Font& font = AssetManagement::SystemFont();

	font(U"Sort & Filter").drawAt(box.topCenter().movedBy(0, 30), Palette::White);

	// Sort display
	font(U"Sort By:").draw(box.x + 30, box.y + 80, Palette::Gray);
	font(m_sortTypes[m_sortTypeIdx]).draw(box.x + 200, box.y + 80, Palette::Yellow);
	font(U"[Up/Down]").draw(box.x + 300, box.y + 80, Palette::Gray);

	// Filter display
	font(U"Level Filter:").draw(box.x + 30, box.y + 140, Palette::Gray);
	String levelStr = m_levelFilter == 0 ? U"ALL" : U"{}"_fmt(m_levelFilter);
	font(levelStr).draw(box.x + 200, box.y + 140, Palette::Yellow);
	font(U"[Left/Right]").draw(box.x + 300, box.y + 140, Palette::Gray);

	font(U"Press Start to apply/close").drawAt(box.bottomCenter().movedBy(0, -30), Palette::Gray);
}

void SelectFilterModal::toggle()
{
	m_isActive = !m_isActive;
}

bool SelectFilterModal::isActive() const
{
	return m_isActive;
}
