#include "OptionMenu.hpp"

namespace
{
	Array<OptionMenuField> MakeFields(const Array<OptionMenuField::CreateInfo>& fieldCreateInfos)
	{
		int32 i = 0;
		return fieldCreateInfos.map([&i, &fieldKeyTexture](const OptionMenuField::CreateInfo& createInfo) {
			if (!createInfo.labelText.empty())
			{
				return OptionMenuField(fieldKeyTexture(0), createInfo); // テクスチャは使用しないが、サイズ情報取得のために0番目のテクスチャを渡す
			}
			const int32 keyTextureIdx = (createInfo.keyTextureIdx == OptionMenuField::CreateInfo::kKeyTextureIdxAutoSet) ? i : createInfo.keyTextureIdx;
			if (createInfo.keyTextureIdx == OptionMenuField::CreateInfo::kKeyTextureIdxAutoSet)
			{
				i++;
			}
			return OptionMenuField(fieldKeyTexture(keyTextureIdx), createInfo);
		});
	}
}

OptionMenu::OptionMenu(const Array<OptionMenuField::CreateInfo>& fieldCreateInfos)
	: m_menu(
		LinearMenu::CreateInfoWithEnumCount{
			.cursorInputCreateInfo = {
				.type = CursorInput::Type::Vertical,
				.buttonFlags = CursorButtonFlags::kArrowOrLaser,
				.buttonIntervalSec = 0.1,
				.buttonIntervalSecFirst = 0.5,
			},
			.enumCount = static_cast<int32>(fieldCreateInfos.size()),
		})
	, m_fields(MakeFields(fieldCreateInfos))
{
	KeyConfig::SaveToConfigIni();
}

bool OptionMenu::update()
{
	m_menu.update();

	const bool cursorChanged = m_menu.deltaCursor() != 0;
	bool valueChanged = false;

	if (!m_fields.empty())
	{
		const int32 cursorIdx = m_menu.cursor();
		const int32 enumCount = static_cast<int32>(m_fields.size());
		if (cursorIdx < 0 || enumCount <= cursorIdx)
		{
			Logger << U"[ksm warning] OptionMenu::update(): Menu cursor is out of range! (func=OptionMenu::update(), cursor={}, min=0, max={})"_fmt(cursorIdx, enumCount - 1);
			return false;
		}
		valueChanged = m_fields[cursorIdx].update();
	}

	return cursorChanged || valueChanged;
}

Array<OptionMenu::SettingItemParams> OptionMenu::getSettingItemParamsList() const
{
	Array<SettingItemParams> result;
	result.reserve(m_fields.size());

	const int32 cursorIdx = m_menu.cursor();
	for (int32 i = 0; i < static_cast<int32>(m_fields.size()); ++i)
	{
		result.push_back({
			.name = m_fields[i].name(),
			.value = m_fields[i].valueDisplayString(),
			.selected = (i == cursorIdx),
			.valueArrowIndex = static_cast<int32>(m_fields[i].arrowType()),
		});
	}

	return result;
}

int32 OptionMenu::cursor() const
{
	return m_menu.cursor();
}

void OptionMenu::setCursor(int32 value)
{
	m_menu.setCursor(value);
}
