#pragma once

namespace I18n
{
	/// @brief 言語ファイルの格納ディレクトリパスを取得
	/// @return 言語ファイルの格納ディレクトリパス
	[[nodiscard]]
	FilePath GetDirectoryPath();

	/// @brief 利用可能な言語リストを取得
	/// @return 利用可能な言語リスト
	[[nodiscard]]
	Array<String> GetAvailableLanguageList();

	/// @brief 標準搭載している言語
	/// @remark ユーザー側で独自に追加したものが選択されている場合はUnknownとなる
	enum class StandardLanguage
	{
		Unknown,
		English,
		Japanese,
		Korean,
		SimplifiedChinese,
		TraditionalChinese,
	};

	/// @brief 現在の言語を取得
	/// @return 現在の言語
	[[nodiscard]]
	StandardLanguage CurrentLanguage();

	/// @brief 言語ファイルを読み込む
	void LoadLanguage(StringView name, StringView fallback = U"English");

	/// @brief カテゴリ名
	enum class Category
	{
		General,
		Select,
		Play,
		Result,
		Option,
		IR,
		InputGate,
	};

	/// @brief 共通/起動時テキストのキー名
	enum class General
	{
		ErrorBassInitFailed,
	};

	/// @brief 選曲画面テキストのキー名
	enum class Select
	{
		Judgment,
		JudgmentBT,
		JudgmentFX,
		JudgmentLaser,
		JudgmentModeOn,
		JudgmentModeOff,
		JudgmentModeAuto,
		JudgmentModeHide,
		EffRate,
		EffRateEasy,
		EffRateNormal,
		EffRateHard,
		Turn,
		TurnNormal,
		TurnMirror,
		TurnRandom,
		PlaybackSpeed,
		AssistTick,
		AssistTickOff,
		AssistTickOn,
		AutoSync,
		AutoSyncOnHigh,
		AutoSyncOnMid,
		AutoSyncOnLow,
		AutoSyncOff,
		FastSlow,
		FastSlowHide,
		FastSlowShow,
		NoteSkin,
		NoteSkinDefault,
		NoteSkinNote,
		Movie,
		MovieOff,
		MovieOn,
		Hispeed,
		PlayStatsPlayCount,
		PlayStatsClear,
		PlayStatsFullCombo,
		PlayStatsPerfect,
		SearchCloseResult,
	};

	/// @brief プレイ画面テキストのキー名
	enum class Play
	{
		ErrorChartFileNotFound,
		ErrorChartLoadFailed,
		ErrorSomeChartMissingInCourse,
		AutoSyncSaveConfirm,
	};

	/// @brief リザルト画面テキストのキー名
	enum class Result
	{
		kErrorFirstMeasureNotFound = 1,
		kErrorUnspecified = 5,
		kDummyTitleChartNotFoundInCourse = 10,
		kErrorChartFileNotFound = 11,
		kErrorLatestVersionRequiredForCourse = 12,
		kErrorSomeChartMissingInCourse = 13,
		kAutoSyncSaveConfirm1 = 50,
		kAutoSyncSaveConfirm2 = 51,
		kAutoSyncSaveConfirmYes = 52,
		kAutoSyncSaveConfirmNo = 53,
		kAutoSyncSaving = 55,
		kAutoSyncInputDelayConfirm = 56,
		kAutoSyncInputDelayConfirmNewVal = 57,
		kAutoSyncInputDelaySaved = 58,
		kUnlockedTrack = 70,
	};

	/// @brief IR関連テキストのキー名
	enum class IR
	{
		kFullscreenOff = 0,
		kFullscreenOn = 1,
		kResolutionX = 2,
		kTextureSizeLarge = 3,
		kTextureSizeMedium = 4,
		kTextureSizeSmall = 5,
		kAlwaysShowOtherFoldersOff = 6,
		kAlwaysShowOtherFoldersOn = 7,
		kHideAllFolderOff = 8,
		kHideAllFolderOn = 9,
		kBGDisplayModeHide = 10,
		kBGDisplayModeShowNoLayer = 11,
		kBGDisplayModeShowLayer = 12,
		kMasterVolumePercent = 13,
		kVsyncOff = 14,
		kVsyncOn = 15,
		kJudgmentOn = 16,
		kJudgmentOff = 17,
		kJudgmentAuto = 18,
		kJudgmentHide = 19,
		kLaserInputTypeKeyboard = 20,
		kLaserInputTypeMouseXY = 22,
		kLaserInputTypeSlider = 24,
		kLaserInputTypeAnalogStickXY = 25,
		kAssistTickOff = 28,
		kAssistTickOn = 29,
		kDisableIMEOff = 30,
		kDisableIMEOnLow = 31,
		kDisableIMEOnMid = 32,
		kDisableIMEOnHigh = 33,
		kTimingAdjustSuffixNoAdjustment = 34,
		kTimingAdjustSuffixLater = 35,
		kTimingAdjustSuffixEarlier = 36,
		kTimingAdjustMs = 37,
		kLaserTimingAdjustLater = 38,
		kLaserTimingAdjustEarlier = 39,
		kLaserMouseDirectionLeftThenRight = 40,
		kLaserMouseDirectionRightThenRight = 41,
		kLaserMouseDirectionUpThenRight = 42,
		kLaserMouseDirectionDownThenRight = 43,
		kLaserSignalSensitivityZero = 44,
		kDisabled = 45,
		kEnabled = 46,
		kSelectCloseFolderKeyBackspace = 47,
		kSelectCloseFolderKeyEsc = 48,
		kHispeedTypeHide = 53,
		kHispeedTypeShow = 54,
		kHideMouseCursorOff = 55,
		kHideMouseCursorOn = 56,
		kUseNumpadAsArrowKeysOff = 57,
		kUseNumpadAsArrowKeysOnKeyboard = 58,
		kUseNumpadAsArrowKeysOnController = 59,
		kKeyConfigKeyboardNoAssign = 61,
		kKeyConfigLaserKeySeparator = 62,
		kKeyConfigGamepadNoAssign = 63,
		kKeyConfigStart = 64,
		kKeyConfigBack = 65,
		kKeyConfigAuto = 66,
		kKeyConfigGamepadHyphen = 67,
		kKeyConfigCategoryPrefixKeyboard = 70,
		kKeyConfigCategoryPrefixGamepad = 71,
		kKeyConfigCategorySuffix = 72,
		kGuideTop = 90,
		kGuideOption = 91,
		kGuideOptionWithRestartRequired = 92,
		kGuideKeyConfig = 93,
		kInternetRanking = 95,
		kInternetRankingOff = 96,
		kInternetRankingOn = 97,
		kGlobalOffset = 98,
		kVisualOffset = 99,
		kAudioProcDelay = 100,
		kAutoPlaySE = 101,
		kMuteAudioInInactiveWindow = 102,
	};

	/// @brief INPUT GATEテキストのキー名
	enum class InputGate
	{
		BulkDownloadConfirm,
	};

	/// @brief OPTION画面テキストのキー名
	enum class Option
	{
		FullscreenOff,
		FullscreenOn,
		TextureSizeLarge,
		TextureSizeMedium,
		TextureSizeSmall,
		AlwaysShowOtherFoldersOff,
		AlwaysShowOtherFoldersOn,
		HideAllFolderOff,
		HideAllFolderOn,
		BGDisplayModeHide,
		BGDisplayModeShowNoLayer,
		BGDisplayModeShowLayer,
		MasterVolumePercent,
		AudioBackendDefault,
		AudioBackendDirectSound,
		VsyncOffWithFps,
		VsyncOn,
		JudgmentOn,
		JudgmentOff,
		JudgmentAuto,
		JudgmentHide,
		LaserInputTypeKeyboard,
		LaserInputTypeMouseXY,
		LaserInputTypeSlider,
		LaserInputTypeAnalogStickXY,
		AssistTickOff,
		AssistTickOn,
		DisableIMEOff,
		DisableIMEOnLow,
		DisableIMEOnMid,
		DisableIMEOnHigh,
		TimingAdjustSuffixNoAdjustment,
		TimingAdjustSuffixLater,
		TimingAdjustSuffixEarlier,
		TimingAdjustMs,
		LaserTimingAdjustSuffixNoAdjustment,
		LaserTimingAdjustSuffixLater,
		LaserTimingAdjustSuffixEarlier,
		VisualOffsetSuffixNoAdjustment,
		VisualOffsetSuffixLater,
		VisualOffsetSuffixEarlier,
		LaserMouseDirectionLeftThenRight,
		LaserMouseDirectionRightThenRight,
		LaserMouseDirectionUpThenRight,
		LaserMouseDirectionDownThenRight,
		Off,
		On,
		SelectCloseFolderKeyBackspace,
		SelectCloseFolderKeyEsc,
		HispeedTypeHide,
		HispeedTypeShow,
		HideMouseCursorOff,
		HideMouseCursorOn,
		UseNumpadAsArrowKeysOff,
		UseNumpadAsArrowKeysOnKeyboard,
		UseNumpadAsArrowKeysOnController,
		KeyConfigNoAssign,
		KeyConfigGamepadButtonFormat,
		KeyConfigStart,
		KeyConfigBack,
		KeyConfigCategoryKeyboard,
		KeyConfigCategoryGamepad,
		GuideTop,
		GuideOption,
		GuideKeyConfig,
		ItemScreenMode,
		ItemScreenResolutionWindow,
		ItemScreenResolutionFullscreen,
		ItemDisplayLanguage,
		ItemGameplayBG,
		ItemAlwaysShowOtherFolders,
		ItemHideAllFolder,
		ItemMasterVolume,
		ItemAudioBackend,
		ItemVsync,
		ItemDefaultBTJudgmentMode,
		ItemDefaultFXJudgmentMode,
		ItemDefaultLaserJudgmentMode,
		ItemLaserInputType,
		ItemAssistTick,
		ItemAutoPlaySE,
		ItemDisableIME,
		ItemTimingAdjustment,
		ItemLaserTimingAdjustment,
		ItemVisualOffset,
		ItemMouseXInputDirection,
		ItemMouseYInputDirection,
		ItemSliderMouseInputSensitivity,
		ItemSwapLRLaser,
		ItemCloseDirectoryKey,
		Item3BTKeysStartBack,
		ItemHispeedTypeXMod,
		ItemHispeedTypeOMod,
		ItemHispeedTypeCMod,
		ItemHideMouseCursor,
		ItemUseNumpadAsArrowKeys,
	};

	/// @brief カテゴリ名とキー名からテキストを直接取得
	/// @param category カテゴリ名
	/// @param key キー名
	/// @return テキスト
	/// @remark 内部実装用。こちらではなくGetを利用すること
	[[nodiscard]]
	StringView GetByCategoryAndKey(const String& category, const String& key);

	/// @brief テキストを取得
	/// @tparam T I18nのenum型
	/// @param key キー
	/// @return テキスト
	template <typename T>
	[[nodiscard]]
	StringView Get(T key)
	{
		if constexpr (std::is_same_v<T, General>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::General), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, Select>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::Select), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, Play>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::Play), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, Result>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::Result), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, Option>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::Option), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, IR>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::IR), EnumUtils::EnumToString(key));
		}
		else if constexpr (std::is_same_v<T, InputGate>)
		{
			return GetByCategoryAndKey(EnumUtils::EnumToString(Category::InputGate), EnumUtils::EnumToString(key));
		}
		else
		{
			static_assert([] { return false; }(), "I18n::Get(): Unsupported enum type.");
			return U"";
		}
	}

	/// @brief フォーマット文字列からテキストを取得
	/// @tparam T I18nのenum型
	/// @tparam ...Args フォーマット引数の型
	/// @param key キー
	/// @param ...args フォーマット引数
	/// @return テキスト
	template <typename T, typename... Args>
	[[nodiscard]]
	String Get(T key, const Args&... args)
	{
		try
		{
			return Fmt(Get(key))(args...);
		}
		catch (const fmt::format_error&)
		{
			return U"[Format Error]";
		}
	}
}
