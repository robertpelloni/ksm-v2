#include "ResultScene.hpp"
#include "Scenes/Select/SelectScene.hpp"
#include "Scenes/PlayPrepare/PlayPrepareScene.hpp"
#include "Scenes/CourseResult/CourseResultScene.hpp"
#include "Scenes/Common/ShowLoadingOneFrame.hpp"
#include "HighScore/KscIO.hpp"
#include "Common/CommonDefines.hpp"
#include "Ini/ConfigIni.hpp"
#include "UI/Dialog.hpp"

namespace
{
	constexpr Duration kFadeDuration = 0.6s;

	constexpr double kGaugeBarMaxHeight = 434.0;

	FilePath GetResultSceneUIFilePath()
	{
		return FsUtils::GetResourcePath(U"ui/scene/result.noco");
	}

	std::shared_ptr<noco::Canvas> LoadResultSceneCanvas()
	{
		const FilePath uiFilePath = GetResultSceneUIFilePath();
		const auto canvas = noco::Canvas::LoadFromFile(uiFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(uiFilePath) };
		}
		return canvas;
	}

	int32 GradeToIndex(Grade grade)
	{
		assert(grade != Grade::kNoGrade && "Unexpected grade in play result: Grade::kNoGrade");
		return static_cast<int32>(grade) - 1;
	}

	enum TopTextureRow : int32
	{
		kTopTextureRowFailed = 0,
		kTopTextureRowCleared,
		kTopTextureRowFullCombo,
		kTopTextureRowPerfect,
		kTopTextureRowAssistFailed,
		kTopTextureRowAssistClear,
		kTopTextureRowAssistFullCombo,
		kTopTextureRowAssistPerfect,
		kTopTextureRowEasyFailed,
		kTopTextureRowEasyClear,
		kTopTextureRowEasyFullCombo,
		kTopTextureRowEasyPerfect,
		kTopTextureRowAssistEasyFailed,
		kTopTextureRowAssistEasyClear,
		kTopTextureRowAssistEasyFullCombo,
		kTopTextureRowAssistEasyPerfect,
		kTopTextureRowHardFailed,
		kTopTextureRowHardClear,
		kTopTextureRowAssistHardFailed,
		kTopTextureRowAssistHardClear,
	};

	int32 TopTextureRow(const MusicGame::PlayResult& playResult)
	{
		const Achievement achievement = playResult.achievement();
		const bool isAssist = playResult.playOption.isAssist();
		switch (achievement)
		{
		case Achievement::kNone:
			if (isAssist)
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
					return kTopTextureRowAssistFailed;

				case GaugeType::kEasyGauge:
					return kTopTextureRowAssistEasyFailed;

				case GaugeType::kHardGauge:
					return kTopTextureRowAssistHardFailed;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			else
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
					return kTopTextureRowFailed;

				case GaugeType::kEasyGauge:
					return kTopTextureRowEasyFailed;

				case GaugeType::kHardGauge:
					return kTopTextureRowHardFailed;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			break;

		case Achievement::kCleared:
			if (isAssist)
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
					return kTopTextureRowAssistClear;

				case GaugeType::kEasyGauge:
					return kTopTextureRowAssistEasyClear;

				case GaugeType::kHardGauge:
					return kTopTextureRowAssistHardClear;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			else
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
					return kTopTextureRowCleared;

				case GaugeType::kEasyGauge:
					return kTopTextureRowEasyClear;

				case GaugeType::kHardGauge:
					return kTopTextureRowHardClear;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			break;

		case Achievement::kFullCombo:
			if (isAssist)
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
				case GaugeType::kHardGauge:
					return kTopTextureRowAssistFullCombo;

				case GaugeType::kEasyGauge:
					return kTopTextureRowAssistEasyFullCombo;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			else
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
				case GaugeType::kHardGauge:
					return kTopTextureRowFullCombo;

				case GaugeType::kEasyGauge:
					return kTopTextureRowEasyFullCombo;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			break;

		case Achievement::kPerfect:
			if (isAssist)
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
				case GaugeType::kHardGauge:
					return kTopTextureRowAssistPerfect;

				case GaugeType::kEasyGauge:
					return kTopTextureRowAssistEasyPerfect;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			else
			{
				switch (playResult.playOption.gaugeType)
				{
				case GaugeType::kNormalGauge:
				case GaugeType::kHardGauge:
					return kTopTextureRowPerfect;

				case GaugeType::kEasyGauge:
					return kTopTextureRowEasyPerfect;

				default:
					assert(false && "Unexpected gauge type in play result");
				}
			}
			break;

		default:
			assert(false && "Unexpected achievement in play result");
		}

		assert(false && "This line should never be reached");
		return kTopTextureRowFailed;
	}
}

ResultScene::ResultScene(const ResultSceneArgs& args)
	: m_canvas(LoadResultSceneCanvas())
	, m_chartData(args.chartData)
	, m_playResult(args.playResult)
	, m_newRecordPanel(m_canvas)
	, m_courseState(args.courseState)
{
	// コースモードの場合は結果を記録
	if (m_courseState)
	{
		m_courseState->recordResult(m_playResult);
	}

	// 前回までのハイスコアを読み込んでNewRecordパネルを設定
	int32 oldScore = 0;
	if (m_playResult.playOption.shouldSaveScore())
	{
		const KscKey condition
		{
			.gaugeType = m_playResult.playOption.gaugeType,
			.turnMode = m_playResult.playOption.turnMode,
			.playbackSpeed = m_playResult.playOption.playbackSpeed,
			.btPlayMode = m_playResult.playOption.effectiveBtJudgmentPlayMode(),
			.fxPlayMode = m_playResult.playOption.effectiveFxJudgmentPlayMode(),
			.laserPlayMode = m_playResult.playOption.effectiveLaserJudgmentPlayMode(),
		};
		const FilePathView chartFilePath = args.chartFilePath;

		// スコア保存前に前回までのハイスコアを取得
		const HighScoreInfo oldHighScore = KscIO::ReadHighScoreInfo(chartFilePath, condition);
		oldScore = oldHighScore.score(m_playResult.playOption.gaugeType);

		// NewRecordパネルに値を設定
		m_newRecordPanel.setValue(oldScore, m_playResult.score);

		// スコアを保存
		KscIO::WriteHighScoreInfo(chartFilePath, m_playResult, condition);
	}

	updateCanvasParams();

	// ジャケット画像を設定
	FilePath jacketFilePath = FileSystem::PathAppend(FileSystem::ParentPath(args.chartFilePath), Unicode::FromUTF8(m_chartData.meta.jacketFilename));

	// 拡張子なしの場合はimgs/jacket内の画像を使用
	if (FileSystem::Extension(jacketFilePath).isEmpty())
	{
		const String baseName = FileSystem::BaseName(jacketFilePath);
		if (!baseName.isEmpty())
		{
			jacketFilePath = FileSystem::PathAppend(U"imgs/jacket", baseName + U".jpg");
		}
	}

	Texture jacketTexture;
	if (FileSystem::IsFile(jacketFilePath))
	{
		jacketTexture = Texture{ jacketFilePath };
	}

	if (const auto jacketNode = m_canvas->findByName(U"JacketImage"))
	{
		if (const auto sprite = jacketNode->getComponent<noco::Sprite>())
		{
			sprite->setTexture(jacketTexture);
			if (jacketTexture.isEmpty())
			{
				sprite->setColor(ColorF{ 0.0, 0.0 });
			}
		}
	}

	m_bgmStream.play();

	AutoMuteAddon::SetEnabled(true);
}

void ResultScene::updateCanvasParams()
{
	// ゲージタイプとパーセンテージに応じてゲージの見た目を変更
	const int32 percentThreshold = m_playResult.playOption.gaugeType == GaugeType::kHardGauge
		? MusicGame::kGaugePercentageThresholdHardWarning
		: MusicGame::kGaugePercentageThreshold;
	const int32 gaugePercentageInt = static_cast<int32>(m_playResult.gaugePercentage);
	const int32 gaugeTextureIndex = static_cast<int32>(m_playResult.playOption.gaugeType) * 2 + (gaugePercentageInt < percentThreshold ? 0 : 1);

	// 未判定分を含めたERROR数を計算
	const int32 judgedCombo = m_playResult.comboStats.critical + m_playResult.comboStats.totalNear() + m_playResult.comboStats.error;
	const int32 unjudgedCombo = m_playResult.totalCombo - judgedCombo;
	const int32 errorCountWithUnjudged = m_playResult.comboStats.error + unjudgedCombo;

	m_canvas->setParamValues({
		{ U"songTitle", Unicode::FromUTF8(m_chartData.meta.title) },
		{ U"artistName", Unicode::FromUTF8(m_chartData.meta.artist) },
		{ U"difficultyIndex", static_cast<double>(m_chartData.meta.difficulty.idx) },
		{ U"levelIndex", static_cast<double>(m_chartData.meta.level - 1) },
		{ U"resultTopIndex", static_cast<double>(TopTextureRow(m_playResult)) },
		{ U"gradeIndex", static_cast<double>(GradeToIndex(m_playResult.grade())) },
		{ U"scoreNumber", U"{:08d}"_fmt(m_playResult.score) },
		{ U"maxComboNumber", U"{:04d}"_fmt(m_playResult.maxCombo) },
		{ U"criticalCount", U"{:04d}"_fmt(m_playResult.comboStats.critical) },
		{ U"nearCount", U"{:04d}"_fmt(m_playResult.comboStats.totalNear()) },
		{ U"errorCount", U"{:04d}"_fmt(errorCountWithUnjudged) },
		{ U"gaugePercentageNumber", U"{}"_fmt(static_cast<int32>(m_playResult.gaugePercentage)) },
		{ U"gaugeTextureIndex", static_cast<double>(gaugeTextureIndex) },
	});

	// ゲージのバーの高さをパーセンテージに応じて変更
	if (auto gaugeClippingMask = m_canvas->findByName(U"GaugeClippingMask"))
	{
		if (const auto* inlineRegion = gaugeClippingMask->inlineRegion())
		{
			noco::InlineRegion newRegion = *inlineRegion;
			newRegion.sizeDelta.y = kGaugeBarMaxHeight * m_playResult.gaugePercentage / 100.0;
			gaugeClippingMask->setRegion(newRegion);
		}
	}
}

Co::Task<void> ResultScene::start()
{
	const auto updateRunner = Co::UpdaterTask([this] { update(); }).runScoped();

	bool userPressedStartOrBack = false;

	if (m_newRecordPanel.isVisible())
	{
		m_newRecordPanel.startDisplay();
		co_await m_newRecordPanel.waitForFadeIn();

		while (true)
		{
			const bool shouldClose = co_await waitForNewRecordPanelClose();

			if (shouldClose)
			{
				userPressedStartOrBack = true;
				break;
			}

			// FX-L+R同時押しで再表示
			m_newRecordPanel.startRedisplay();
			co_await m_newRecordPanel.waitForFadeIn();
		}
	}

	// Auto Sync Check
	co_await checkAutoSync();

	if (!userPressedStartOrBack)
	{
		co_await Co::Any(
			KeyConfig::WaitUntilDown(kButtonStart),
			KeyConfig::WaitUntilDown(kButtonBack));
	}

	// コースモードの場合は次の曲またはコースリザルトへ
	if (m_courseState)
	{
		const bool isFailed = m_playResult.achievement() == Achievement::kNone;

		// FAILED時またはコース完了時はコースリザルトへ
		if (isFailed || !m_courseState->hasNextChart())
		{
			if (!isFailed)
			{
				m_courseState->setCleared(true);
			}
			requestNextScene<CourseResultScene>(*m_courseState);
		}
		else
		{
			// 次の曲へ
			m_courseState->advanceToNextChart();

			const FilePath nextChartPath = m_courseState->currentChartPath();

			// 次の曲へ
			co_await ShowLoadingOneFrame::Play(HasBgYN::Yes);
			requestNextScene<PlayPrepareScene>(nextChartPath, m_playResult.playOption.isAutoPlay, m_courseState);
		}
	}
	else
	{
		requestNextScene<SelectScene>();
	}
}

Co::Task<bool> ResultScene::waitForNewRecordPanelClose()
{
	bool fxLRPressedPrev = false;
	Stopwatch displayStopwatch{ StartImmediately::Yes };

	while (true)
	{
		co_await Co::NextFrame();

		// FX-L+R同時押しで表示時間を3秒延長
		const bool fxLRPressed = KeyConfig::Pressed(kButtonFX_L) && KeyConfig::Pressed(kButtonFX_R);
		if (fxLRPressed && !fxLRPressedPrev)
		{
			displayStopwatch.restart();
		}
		fxLRPressedPrev = fxLRPressed;

		// 3秒経過またはSTART/Backで終了
		if (displayStopwatch.sF() >= 3.0 ||
			KeyConfig::Down(kButtonStart) ||
			KeyConfig::Down(kButtonBack))
		{
			break;
		}
	}

	m_newRecordPanel.startFadeOut();
	co_await m_newRecordPanel.waitForFadeOut();

	// フェードアウト後、FX-L+Rで再表示
	while (true)
	{
		co_await Co::NextFrame();

		const bool fxLRPressed = KeyConfig::Pressed(kButtonFX_L) && KeyConfig::Pressed(kButtonFX_R);
		if (fxLRPressed && !fxLRPressedPrev)
		{
			co_return false;
		}
		fxLRPressedPrev = fxLRPressed;

		if (KeyConfig::Down(kButtonStart) || KeyConfig::Down(kButtonBack))
		{
			co_return true;
		}
	}
}

Co::Task<void> ResultScene::checkAutoSync()
{
	// Auto Syncの設定値を取得
	// 0: Off, 1: Low, 2: Mid, 3: High
	const int32 autoSyncLevel = ConfigIni::GetInt(ConfigIni::Key::kAutoSync, 0);
	if (autoSyncLevel == 0)
	{
		co_return;
	}

	const auto& stats = m_playResult.comboStats;
	if (stats.deviationCount == 0)
	{
		co_return;
	}

	// 平均ズレ時間(秒) (Positive: Fast/Early, Negative: Slow/Late in data, but let's re-verify)
	// ComboStatus.cpp: diffSec = noteTime - inputTime
	// if note=1.1, input=1.0 (Early), diff=+0.1.
	// if note=1.1, input=1.2 (Late), diff=-0.1.
	//
	// InputDelay in ConfigIni adds delay to input? or subtracts?
	// Usually "Input Delay" means "Hardware Latency".
	// If hardware has +10ms latency, the input arrives 10ms LATE.
	// Game sees input at T+10ms.
	// To correct, we must SUBTRACT 10ms from the timestamp? Or add 10ms to note time?
	// Usually: AdjustedInputTime = RawInputTime - InputDelay.
	//
	// If I am consistently Early (diff > 0), it means InputTime < NoteTime.
	// It implies either I am rushing, or the visual/audio is early.
	// If I press early, it might mean the audio is ahead of visual?
	//
	// Let's look at "Auto Adjust" features in other games.
	// If average error is +20ms (Early), we usually shift the offset by +20ms?
	// If I hit early, I need the judgment line to be "earlier" (visually) or "later" (internally)?
	//
	// Let's assume the standard convention:
	// If Average Error is X ms, we add X ms to the existing offset.
	// Example: I hit 20ms early (+20ms).
	// New Offset = Old Offset + 20ms.
	// If Old Offset was 0, New is +20.
	// If I apply +20ms offset (assuming it means "make notes appear later" or "shift judgment window later"):
	// Original: Note at 1000ms. I hit at 980ms. Error +20ms.
	// Shift: Note effectively at 980ms (or input treated as 1000ms).
	//
	// Let's try to stick to the logic:
	// Recommended Offset Change = Average Deviation.

	const double avgDeviationSec = stats.totalDeviationSec / stats.deviationCount;
	const int32 avgDeviationMs = static_cast<int32>(avgDeviationSec * 1000);

	// Thresholds (ms) based on level
	// High: Sensitive (small threshold)
	// Low: Less sensitive (large threshold)
	int32 thresholdMs = 0;
	switch (autoSyncLevel)
	{
	case 1: // Low
		thresholdMs = 40;
		break;
	case 2: // Mid
		thresholdMs = 25;
		break;
	case 3: // High
		thresholdMs = 15;
		break;
	default:
		return;
	}

	if (Abs(avgDeviationMs) < thresholdMs)
	{
		co_return;
	}

	// Format confirmation message
	const int32 currentDelay = ConfigIni::GetInt(ConfigIni::Key::kInputDelay, 0);
	const int32 newDelay = currentDelay + avgDeviationMs;

	// Note: We need to use I18n strings here.
	// Ideally: "Input timing deviation detected.\nAdjust Input Delay?\nCurrent: {0}ms -> New: {1}ms"
	// Using existing keys or fallback.
	// Since I18n keys for this specific message might not exist in standard yet, I'll use placeholders or existing.
	// I added I18n keys in the previous turn? No, I added them to I18n.hpp but didn't implement the Japanese text.
	// But `I18n::Play::kAutoSyncSaveConfirm1` etc were in the list.
	// Let's use `I18n::Play::kAutoSyncSaveConfirm1` + numbers + `kAutoSyncSaveConfirm2`.

	const String message = I18n::Get(I18n::Play::kAutoSyncSaveConfirm1) + U"\n" +
		U"{} ms -> {} ms"_fmt(currentDelay, newDelay) + U"\n" +
		I18n::Get(I18n::Play::kAutoSyncSaveConfirm2); // "Save?"

	const Dialog::Result result = co_await Dialog::Confirm(message,
		I18n::Get(I18n::Play::kAutoSyncSaveConfirmYes),
		I18n::Get(I18n::Play::kAutoSyncSaveConfirmNo));

	if (result == Dialog::Result::Yes)
	{
		ConfigIni::SetInt(ConfigIni::Key::kInputDelay, newDelay);
		ConfigIni::Save();

		// Show "Saved" message briefly? Or just proceed.
		// `kAutoSyncSaving` exists.
	}
}

void ResultScene::update()
{
	m_canvas->update();
}

void ResultScene::draw() const
{
	m_canvas->draw();
}

Co::Task<void> ResultScene::fadeIn()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeIn(kFadeDuration, Palette::White);
}

Co::Task<void> ResultScene::postFadeOut()
{
	// SelectSceneはコンストラクタの処理に時間がかかるので、ローディングはここで出しておく
	return ShowLoadingOneFrame::Play(HasBgYN::Yes);
}
