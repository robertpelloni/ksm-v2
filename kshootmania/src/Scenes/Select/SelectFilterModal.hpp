#pragma once
#include <Siv3D.hpp>

class SelectFilterModal
{
private:
	bool m_isActive = false;

	// Simple state for currently selected sort
	int32 m_sortTypeIdx = 0;
	Array<String> m_sortTypes = { U"Title", U"Level" };

	// Filter level
	int32 m_levelFilter = 0; // 0 means ALL

public:
	SelectFilterModal() = default;

	void update();
	void draw() const;

	void toggle();
	bool isActive() const;

	int32 getSortType() const { return m_sortTypeIdx; }
	int32 getLevelFilter() const { return m_levelFilter; }
};
