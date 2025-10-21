#include "bt_option_panel.hpp"
#include "common/common_defines.hpp"
#include "common/math_utils.hpp"
#include "i18n/i18n.hpp"
#include "ini/config_ini.hpp"
#include "input/cursor/cursor_input.hpp"

namespace
{
	using MusicGame::HispeedType;

	// BT-Aメニューの項目
	enum class BTAMenuItem : int32
	{
		kJudgmentBT = 0,
		kJudgmentFX = 1,
		kJudgmentLaser = 2,

		kCount,
	};

	// BT-Bメニューの項目
	enum class BTBMenuItem : int32
	{
		kEffRate = 0,
		kTurn = 1,

		kCount,
	};

	// BT-Cメニューの項目
	enum class BTCMenuItem : int32
	{
		kAssistTick = 0,
		kAutoSync = 1,
		kFastSlow = 2,
		kNoteSkin = 3,
		kMovie = 4,

		kCount,
	};




	// ハイスピード値の範囲
	constexpr int32 kHispeedXModMin = 1; // x-modの最小値(x0.1)
	constexpr int32 kHispeedXModMax = 99; // x-modの最大値(x9.9)
	constexpr int32 kHispeedOCModMin = 25; // o/c-modの最小値
	constexpr int32 kHispeedOCModMax = 2000; // o/c-modの最大値
	constexpr int32 kHispeedOCModStep = 25; // o/c-modの刻み幅

	// ConfigIniから「表示」設定になっているハイスピード種類を取得
	Array<HispeedType> LoadAvailableHispeedTypesFromConfigIni()
	{
		Array<HispeedType> availableTypes;
		availableTypes.reserve(static_cast<std::size_t>(HispeedType::EnumCount));

		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowXMod, true))
		{
			availableTypes.push_back(HispeedType::XMod);
		}
		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowOMod, true))
		{
			availableTypes.push_back(HispeedType::OMod);
		}
		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowCMod, false))
		{
			availableTypes.push_back(HispeedType::CMod);
		}

		// 1つも有効でない場合はデフォルト(x-mod、o-mod)を追加
		if (availableTypes.empty())
		{
			availableTypes.push_back(HispeedType::XMod);
			availableTypes.push_back(HispeedType::OMod);
		}

		return availableTypes;
	}

	int32 CursorMin(HispeedType hispeedType)
	{
		switch (hispeedType)
		{
		case HispeedType::XMod:
			return kHispeedXModMin;
		case HispeedType::OMod:
		case HispeedType::CMod:
			return kHispeedOCModMin;
		default:
			return kHispeedOCModMin;
		}
	}

	int32 CursorMax(HispeedType hispeedType)
	{
		switch (hispeedType)
		{
		case HispeedType::XMod:
			return kHispeedXModMax;
		case HispeedType::OMod:
		case HispeedType::CMod:
			return kHispeedOCModMax;
		default:
			return kHispeedOCModMax;
		}
	}

	int32 CursorStep(HispeedType hispeedType)
	{
		switch (hispeedType)
		{
		case HispeedType::XMod:
			return 1;
		case HispeedType::OMod:
		case HispeedType::CMod:
			return kHispeedOCModStep;
		default:
			return kHispeedOCModStep;
		}
	}

	StringView JudgmentModeToI18nKey(ConfigIni::Value::JudgmentMode::JudgmentMode mode)
	{
		switch (mode)
		{
		case ConfigIni::Value::JudgmentMode::kOn: return I18n::Get(I18n::Select::kJudgmentModeOn);
		case ConfigIni::Value::JudgmentMode::kOff: return I18n::Get(I18n::Select::kJudgmentModeOff);
		case ConfigIni::Value::JudgmentMode::kAuto: return I18n::Get(I18n::Select::kJudgmentModeAuto);
		case ConfigIni::Value::JudgmentMode::kHide: return I18n::Get(I18n::Select::kJudgmentModeHide);
		default: return U"";
		}
	}

	StringView GaugeTypeToI18nKey(GaugeType gauge)
	{
		switch (gauge)
		{
		case GaugeType::kEasyGauge: return I18n::Get(I18n::Select::kEffRateEasy);
		case GaugeType::kNormalGauge: return I18n::Get(I18n::Select::kEffRateNormal);
		case GaugeType::kHardGauge: return I18n::Get(I18n::Select::kEffRateHard);
		default: return U"";
		}
	}

	StringView TurnModeToI18nKey(TurnMode turn)
	{
		switch (turn)
		{
		case TurnMode::kNormal: return I18n::Get(I18n::Select::kTurnNormal);
		case TurnMode::kMirror: return I18n::Get(I18n::Select::kTurnMirror);
		case TurnMode::kRandom: return I18n::Get(I18n::Select::kTurnRandom);
		default: return U"";
		}
	}

	StringView AssistTickModeToI18nKey(AssistTickMode assistTick)
	{
		switch (assistTick)
		{
		case AssistTickMode::kOff: return I18n::Get(I18n::Select::kAssistTickOff);
		case AssistTickMode::kOn: return I18n::Get(I18n::Select::kAssistTickOn);
		default: return U"";
		}
	}

	StringView AutoSyncModeToI18nKey(AutoSyncMode autoSync)
	{
		switch (autoSync)
		{
		case AutoSyncMode::kOff: return I18n::Get(I18n::Select::kAutoSyncOff);
		case AutoSyncMode::kLow: return I18n::Get(I18n::Select::kAutoSyncOnLow);
		case AutoSyncMode::kMid: return I18n::Get(I18n::Select::kAutoSyncOnMid);
		case AutoSyncMode::kHigh: return I18n::Get(I18n::Select::kAutoSyncOnHigh);
		default: return U"";
		}
	}

	StringView FastSlowModeToI18nKey(FastSlowMode display)
	{
		switch (display)
		{
		case FastSlowMode::kHide: return I18n::Get(I18n::Select::kFastSlowHide);
		case FastSlowMode::kShow: return I18n::Get(I18n::Select::kFastSlowShow);
		default: return U"";
		}
	}

	StringView NoteSkinTypeToI18nKey(NoteSkinType skin)
	{
		switch (skin)
		{
		case NoteSkinType::kDefault: return I18n::Get(I18n::Select::kNoteSkinDefault);
		case NoteSkinType::kNote: return I18n::Get(I18n::Select::kNoteSkinNote);
		default: return U"";
		}
	}

	StringView MovieModeToI18nKey(MovieMode display)
	{
		switch (display)
		{
		case MovieMode::kOff: return I18n::Get(I18n::Select::kMovieOff);
		case MovieMode::kOn: return I18n::Get(I18n::Select::kMovieOn);
		default: return U"";
		}
	}

	// ハイスピード値をフォーマット
	String FormatHispeedValue(HispeedType type, int32 value)
	{
		switch (type)
		{
		case HispeedType::XMod:
			return U"x{:0>2}"_fmt(value); // x01, x10など2桁ゼロ埋め
		case HispeedType::OMod:
			return U"{}"_fmt(value);
		case HispeedType::CMod:
			return U"C{}"_fmt(value);
		default:
			return U"";
		}
	}

	String FormatMenuLine(StringView label, StringView value, bool isSelected, int32 currentCursor, int32 minCursor, int32 maxCursor)
	{
		String line{ label };

		// 左端の場合は<を表示しない
		const bool showLeft = isSelected && currentCursor > minCursor;
		// 右端の場合は>を表示しない
		const bool showRight = isSelected && currentCursor < maxCursor;

		line += showLeft ? U"<" : U" ";
		line += value;
		if (showRight)
		{
			line += U">";
		}
		return line;
	}
}

BTOptionPanel::BTOptionPanel(std::shared_ptr<noco::Canvas> canvas)
	: m_canvas(canvas)
	, m_btAMenu(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Vertical,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(BTAMenuItem::kCount),
		.cyclic = IsCyclicMenuYN::Yes,
	})
	, m_btBMenu(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Vertical,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(BTBMenuItem::kCount),
		.cyclic = IsCyclicMenuYN::Yes,
	})
	, m_btCMenu(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Vertical,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(BTCMenuItem::kCount),
		.cyclic = IsCyclicMenuYN::Yes,
	})

	// BT-Aメニューの値変更用
	, m_judgmentModeBT(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = 4, // JudgmentModeの値の数(On, Off, Auto, Hide)
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_judgmentModeFX(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = 4, // JudgmentModeの値の数(On, Off, Auto, Hide)
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_judgmentModeLaser(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = 4, // JudgmentModeの値の数(On, Off, Auto, Hide)
		.cyclic = IsCyclicMenuYN::No,
	})

	// BT-Bメニューの値変更用
	, m_gaugeType(LinearMenu::CreateInfoWithCursorMinMax{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.cursorMin = static_cast<int32>(GaugeType::kEasyGauge),
		.cursorMax = static_cast<int32>(GaugeType::kHardGauge),
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_turnMode(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(3),
		.cyclic = IsCyclicMenuYN::No,
	})

	// BT-Cメニューの値変更用
	, m_assistTick(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(AssistTickMode::kCount),
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_autoSync(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(AutoSyncMode::kCount),
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_fastSlow(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(FastSlowMode::kCount),
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_noteSkin(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(NoteSkinType::kCount),
		.cyclic = IsCyclicMenuYN::No,
	})
	, m_movie(LinearMenu::CreateInfoWithEnumCount{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrow,
		},
		.enumCount = static_cast<int32>(MovieMode::kCount),
		.cyclic = IsCyclicMenuYN::No,
	})

	// BT-Dメニュー(ハイスピード)用
	, m_hispeedTypeMenu(
		LoadAvailableHispeedTypesFromConfigIni(),
		LinearMenu::CreateInfoWithCursorMinMax{
			.cursorInputCreateInfo = {
				.type = CursorInput::Type::Horizontal,
				.buttonFlags = CursorButtonFlags::kArrow,
				.buttonIntervalSec = 0.12,
			},
			.cyclic = IsCyclicMenuYN::No,
		})
	, m_hispeedValueMenu(
		LinearMenu::CreateInfoWithCursorMinMax{
			.cursorInputCreateInfo = {
				.type = CursorInput::Type::Vertical,
				.buttonFlags = CursorButtonFlags::kArrow,
				.flipArrowKeyDirection = FlipArrowKeyDirectionYN::Yes, // 上向きで増加、下向きで減少
				.buttonIntervalSec = 0.06,
			},
			.cursorMin = 0,
			.cursorMax = 0,
			.cyclic = IsCyclicMenuYN::No,
		})
{

	// ハイスピードのデフォルト値を設定
	m_hispeedValueMenu.setCursor(10); // x1.0
	refreshHispeedValueMenuConstraints();

	// ConfigIniから設定値を読み込み
	loadFromConfigIni();
}

Optional<KeyConfig::Button> BTOptionPanel::getCurrentSingleBTButton() const
{
	int32 pressedCount = 0;
	Optional<KeyConfig::Button> pressedButton;

	for (KeyConfig::Button button = KeyConfig::kBT_A; button <= KeyConfig::kBT_D; ++button)
	{
		if (KeyConfig::Pressed(button))
		{
			++pressedCount;
			pressedButton = button;
		}
	}

	// 単独押しの場合のみ返す
	if (pressedCount == 1)
	{
		return pressedButton;
	}

	return none;
}

void BTOptionPanel::refreshHispeedValueMenuConstraints()
{
	const HispeedType hispeedType = m_hispeedTypeMenu.cursorValue();
	m_hispeedValueMenu.setCursorMinMax(CursorMin(hispeedType), CursorMax(hispeedType));
	m_hispeedValueMenu.setCursorStep(CursorStep(hispeedType));
}

String BTOptionPanel::generateBTAMenuText() const
{
	const auto judgmentModeBT = m_judgmentModeBT.cursorAs<ConfigIni::Value::JudgmentMode::JudgmentMode>();
	const auto judgmentModeFX = m_judgmentModeFX.cursorAs<ConfigIni::Value::JudgmentMode::JudgmentMode>();
	const auto judgmentModeLaser = m_judgmentModeLaser.cursorAs<ConfigIni::Value::JudgmentMode::JudgmentMode>();

	const auto currentItem = m_btAMenu.cursorAs<BTAMenuItem>();

	String text{ I18n::Get(I18n::Select::kJudgment) };
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kJudgmentBT), JudgmentModeToI18nKey(judgmentModeBT), currentItem == BTAMenuItem::kJudgmentBT, m_judgmentModeBT.cursor(), 0, 3);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kJudgmentFX), JudgmentModeToI18nKey(judgmentModeFX), currentItem == BTAMenuItem::kJudgmentFX, m_judgmentModeFX.cursor(), 0, 3);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kJudgmentLaser), JudgmentModeToI18nKey(judgmentModeLaser), currentItem == BTAMenuItem::kJudgmentLaser, m_judgmentModeLaser.cursor(), 0, 3);

	return text;
}

String BTOptionPanel::generateBTBMenuText() const
{
	const auto gaugeType = m_gaugeType.cursorAs<GaugeType>();
	const auto turnMode = m_turnMode.cursorAs<TurnMode>();

	const auto currentItem = m_btBMenu.cursorAs<BTBMenuItem>();

	String text = U"";
	text += FormatMenuLine(I18n::Get(I18n::Select::kEffRate), GaugeTypeToI18nKey(gaugeType), currentItem == BTBMenuItem::kEffRate, m_gaugeType.cursor(), static_cast<int32>(GaugeType::kEasyGauge), static_cast<int32>(GaugeType::kHardGauge));
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kTurn), TurnModeToI18nKey(turnMode), currentItem == BTBMenuItem::kTurn, m_turnMode.cursor(), 0, 2);

	return text;
}

String BTOptionPanel::generateBTCMenuText() const
{
	const auto assistTick = m_assistTick.cursorAs<AssistTickMode>();
	const auto autoSync = m_autoSync.cursorAs<AutoSyncMode>();
	const auto fastSlow = m_fastSlow.cursorAs<FastSlowMode>();
	const auto noteSkin = m_noteSkin.cursorAs<NoteSkinType>();
	const auto movie = m_movie.cursorAs<MovieMode>();

	const auto currentItem = m_btCMenu.cursorAs<BTCMenuItem>();

	String text = U"";
	text += FormatMenuLine(I18n::Get(I18n::Select::kAssistTick), AssistTickModeToI18nKey(assistTick), currentItem == BTCMenuItem::kAssistTick, m_assistTick.cursor(), 0, static_cast<int32>(AssistTickMode::kCount) - 1);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kAutoSync), AutoSyncModeToI18nKey(autoSync), currentItem == BTCMenuItem::kAutoSync, m_autoSync.cursor(), 0, static_cast<int32>(AutoSyncMode::kCount) - 1);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kFastSlow), FastSlowModeToI18nKey(fastSlow), currentItem == BTCMenuItem::kFastSlow, m_fastSlow.cursor(), 0, static_cast<int32>(FastSlowMode::kCount) - 1);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kNoteSkin), NoteSkinTypeToI18nKey(noteSkin), currentItem == BTCMenuItem::kNoteSkin, m_noteSkin.cursor(), 0, static_cast<int32>(NoteSkinType::kCount) - 1);
	text += U"\n";
	text += FormatMenuLine(I18n::Get(I18n::Select::kMovie), MovieModeToI18nKey(movie), currentItem == BTCMenuItem::kMovie, m_movie.cursor(), 0, static_cast<int32>(MovieMode::kCount) - 1);

	return text;
}

String BTOptionPanel::generateBTDMenuText() const
{
	const auto hispeedType = m_hispeedTypeMenu.cursorValue();
	const int32 hispeedValue = m_hispeedValueMenu.cursor();

	String text{ I18n::Get(I18n::Select::kHispeed) };
	text += U"\n";
	text += U"          ";
	text += FormatHispeedValue(hispeedType, hispeedValue);

	return text;
}

void BTOptionPanel::update()
{
	const auto currentButton = getCurrentSingleBTButton();

	// いずれかのBTボタンが単独で押されている場合
	if (currentButton)
	{
		m_isVisible = true;
		m_canvas->setParamValue(U"overlay_visible", true);

		// 対応するパネルを表示
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_A", currentButton == KeyConfig::kBT_A);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_B", currentButton == KeyConfig::kBT_B);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_C", currentButton == KeyConfig::kBT_C);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_D", currentButton == KeyConfig::kBT_D);

		// 各メニューの更新
		if (currentButton == KeyConfig::kBT_A)
		{
			m_btAMenu.update();

			// 現在選択されている項目の値変更用LinearMenuを更新
			const auto currentItem = m_btAMenu.cursorAs<BTAMenuItem>();
			bool valueChanged = false;

			if (currentItem == BTAMenuItem::kJudgmentBT)
			{
				m_judgmentModeBT.update();
				valueChanged = m_judgmentModeBT.deltaCursor() != 0;
			}
			else if (currentItem == BTAMenuItem::kJudgmentFX)
			{
				m_judgmentModeFX.update();
				valueChanged = m_judgmentModeFX.deltaCursor() != 0;
			}
			else if (currentItem == BTAMenuItem::kJudgmentLaser)
			{
				m_judgmentModeLaser.update();
				valueChanged = m_judgmentModeLaser.deltaCursor() != 0;
			}

			if (valueChanged)
			{
				saveToConfigIni();
			}

			m_canvas->setParamValue(U"overlay_btOptionPanelText_A", generateBTAMenuText());
		}
		else if (currentButton == KeyConfig::kBT_B)
		{
			m_btBMenu.update();

			// 現在選択されている項目の値変更用LinearMenuを更新
			const auto currentItem = m_btBMenu.cursorAs<BTBMenuItem>();
			bool valueChanged = false;

			if (currentItem == BTBMenuItem::kEffRate)
			{
				m_gaugeType.update();
				valueChanged = m_gaugeType.deltaCursor() != 0;
			}
			else if (currentItem == BTBMenuItem::kTurn)
			{
				m_turnMode.update();
				valueChanged = m_turnMode.deltaCursor() != 0;
			}

			if (valueChanged)
			{
				saveToConfigIni();
			}

			m_canvas->setParamValue(U"overlay_btOptionPanelText_B", generateBTBMenuText());
		}
		else if (currentButton == KeyConfig::kBT_C)
		{
			m_btCMenu.update();

			// 現在選択されている項目の値変更用LinearMenuを更新
			const auto currentItem = m_btCMenu.cursorAs<BTCMenuItem>();
			bool valueChanged = false;

			if (currentItem == BTCMenuItem::kAssistTick)
			{
				m_assistTick.update();
				valueChanged = m_assistTick.deltaCursor() != 0;
			}
			else if (currentItem == BTCMenuItem::kAutoSync)
			{
				m_autoSync.update();
				valueChanged = m_autoSync.deltaCursor() != 0;
			}
			else if (currentItem == BTCMenuItem::kFastSlow)
			{
				m_fastSlow.update();
				valueChanged = m_fastSlow.deltaCursor() != 0;
			}
			else if (currentItem == BTCMenuItem::kNoteSkin)
			{
				m_noteSkin.update();
				valueChanged = m_noteSkin.deltaCursor() != 0;
			}
			else if (currentItem == BTCMenuItem::kMovie)
			{
				m_movie.update();
				valueChanged = m_movie.deltaCursor() != 0;
			}

			if (valueChanged)
			{
				saveToConfigIni();
			}

			m_canvas->setParamValue(U"overlay_btOptionPanelText_C", generateBTCMenuText());
		}
		else if (currentButton == KeyConfig::kBT_D)
		{
			m_hispeedTypeMenu.update();
			m_hispeedValueMenu.update();

			bool valueChanged = false;

			// 種類が変更された場合、値の範囲を更新し、現在の値に最も近い値に設定
			if (m_hispeedTypeMenu.deltaCursor() != 0)
			{
				refreshHispeedValueMenuConstraints();

				// TODO: 実際のBPMを取得して、現在の値に最も近い値を計算
				const HispeedType newType = m_hispeedTypeMenu.cursorValue();
				if (newType == HispeedType::XMod)
				{
					m_hispeedValueMenu.setCursor(10); // x1.0
				}
				else
				{
					m_hispeedValueMenu.setCursor(150); // 150 or C150
				}

				valueChanged = true;
			}
			else if (m_hispeedValueMenu.deltaCursor() != 0)
			{
				valueChanged = true;
			}

			if (valueChanged)
			{
				saveToConfigIni();
			}

			m_canvas->setParamValue(U"overlay_btOptionPanelText_D", generateBTDMenuText());
		}
	}
	else
	{
		m_isVisible = false;

		// BTボタンが押されていない場合はすべて非表示
		m_canvas->setParamValue(U"overlay_visible", false);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_A", false);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_B", false);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_C", false);
		m_canvas->setParamValue(U"overlay_btOptionPanelVisible_D", false);
	}
}

bool BTOptionPanel::isVisible() const
{
	return m_isVisible;
}

void BTOptionPanel::loadFromConfigIni()
{

	// BT-Aメニューの設定を読み込み
	m_judgmentModeBT.setCursor(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeBT, ConfigIni::Value::JudgmentMode::kOn));
	m_judgmentModeFX.setCursor(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeFX, ConfigIni::Value::JudgmentMode::kOn));
	m_judgmentModeLaser.setCursor(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeLaser, ConfigIni::Value::JudgmentMode::kOn));

	// BT-Bメニューの設定を読み込み
	m_gaugeType.setCursor(ConfigIni::GetInt(ConfigIni::Key::kEffRateType, static_cast<int32>(GaugeType::kNormalGauge)));
	m_turnMode.setCursor(ConfigIni::GetInt(ConfigIni::Key::kTurn, static_cast<int32>(TurnMode::kNormal)));

	// BT-Cメニューの設定を読み込み
	m_assistTick.setCursor(ConfigIni::GetInt(ConfigIni::Key::kAssistTick, static_cast<int32>(AssistTickMode::kOff)));

	// TODO: AutoSync設定を実装
	m_fastSlow.setCursor(ConfigIni::GetInt(ConfigIni::Key::kShowFastSlow, static_cast<int32>(FastSlowMode::kHide)));

	// noteskinは文字列として保存される
	const StringView noteSkinStr = ConfigIni::GetString(ConfigIni::Key::kNoteSkin, U"default");
	if (noteSkinStr == U"note")
	{
		m_noteSkin.setCursor(static_cast<int32>(NoteSkinType::kNote));
	}
	else
	{
		m_noteSkin.setCursor(static_cast<int32>(NoteSkinType::kDefault));
	}

	m_movie.setCursor(ConfigIni::GetInt(ConfigIni::Key::kBGMovie, static_cast<int32>(MovieMode::kOn)));

	// BT-Dメニュー(ハイスピード)の設定を読み込み
	// TODO: ハイスピード値の読み込み処理を実装
}

void BTOptionPanel::saveToConfigIni()
{

	// BT-Aメニューの設定を保存
	ConfigIni::SetInt(ConfigIni::Key::kJudgmentModeBT, m_judgmentModeBT.cursor());
	ConfigIni::SetInt(ConfigIni::Key::kJudgmentModeFX, m_judgmentModeFX.cursor());
	ConfigIni::SetInt(ConfigIni::Key::kJudgmentModeLaser, m_judgmentModeLaser.cursor());

	// BT-Bメニューの設定を保存
	ConfigIni::SetInt(ConfigIni::Key::kEffRateType, m_gaugeType.cursor());
	ConfigIni::SetInt(ConfigIni::Key::kTurn, m_turnMode.cursor());

	// BT-Cメニューの設定を保存
	ConfigIni::SetInt(ConfigIni::Key::kAssistTick, m_assistTick.cursor());

	// TODO: AutoSync設定を実装
	ConfigIni::SetInt(ConfigIni::Key::kShowFastSlow, m_fastSlow.cursor());

	// noteskinは文字列として保存
	if (m_noteSkin.cursorAs<NoteSkinType>() == NoteSkinType::kNote)
	{
		ConfigIni::SetString(ConfigIni::Key::kNoteSkin, U"note");
	}
	else
	{
		ConfigIni::SetString(ConfigIni::Key::kNoteSkin, U"default");
	}

	ConfigIni::SetInt(ConfigIni::Key::kBGMovie, m_movie.cursor());

	// BT-Dメニュー(ハイスピード)の設定を保存
	// TODO: ハイスピード値の保存処理を実装

	// ConfigIniをファイルに書き込み
	ConfigIni::Save();
}
