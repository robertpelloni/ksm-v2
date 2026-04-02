#pragma once
#include <Siv3D.hpp>

namespace UI
{
	class PopupDialog
	{
	private:
		String m_title;
		String m_message;
		bool m_isActive = false;
		double m_timeActive = 0.0;

	public:
		PopupDialog() = default;

		void show(StringView title, StringView message)
		{
			m_title = title;
			m_message = message;
			m_isActive = true;
			m_timeActive = 0.0;
		}

		void update()
		{
			if (m_isActive)
			{
				m_timeActive += Scene::DeltaTime();
			}
		}

		void draw() const
		{
			if (!m_isActive) return;

			// Simple fade in/out animation
			double alpha = 1.0;
			if (m_timeActive < 0.2)
			{
				alpha = m_timeActive / 0.2;
			}

			// Screen overlay
			Scene::Rect().draw(ColorF(0.0, 0.7 * alpha));

			// Dialog box
			const RectF box(Arg::center = Scene::Center(), 500, 250);
			box.draw(ColorF(0.1, alpha)).drawFrame(2, ColorF(Palette::Cyan, alpha));

			// Text
			const Font& titleFont = SimpleGUI::GetFont(); // Assuming we want quick system font here or AssetManagement
			titleFont(m_title).drawAt(box.topCenter().movedBy(0, 40), ColorF(Palette::Cyan, alpha));
			titleFont(m_message).drawAt(box.center(), ColorF(Palette::White, alpha));

			// Button prompt
			titleFont(U"[START] OK").drawAt(box.bottomCenter().movedBy(0, -30), ColorF(Palette::Gray, alpha));
		}

		bool isActive() const
		{
			return m_isActive;
		}

		void dismiss()
		{
			m_isActive = false;
		}
	};
}
