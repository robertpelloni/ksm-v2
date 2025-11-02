#include "result_scene.hpp"
#include "scene/select/select_scene.hpp"
#include "scene/common/show_loading_one_frame.hpp"
#include "high_score/ksc_io.hpp"
#include "common/common_defines.hpp"

namespace
{
	constexpr Duration kFadeDuration = 0.6s;

	constexpr FilePathView kResultSceneUIFilePath = U"ui/scene/result.noco";

	constexpr double kGaugeBarMaxHeight = 434.0;

	std::shared_ptr<noco::Canvas> LoadResultSceneCanvas()
	{
		const auto canvas = noco::Canvas::LoadFromFile(kResultSceneUIFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(kResultSceneUIFilePath) };
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
{
	if (!m_playResult.playOption.isAutoPlay) // オートプレイの場合はスコアを保存しない(オートプレイではリザルト画面を出さないので不要だが一応チェックはする)
	{
		const KscKey condition
		{
			.gaugeType = m_playResult.playOption.gaugeType,
			.turnMode = m_playResult.playOption.turnMode,
			.btPlayMode = m_playResult.playOption.effectiveBtJudgmentPlayMode(),
			.fxPlayMode = m_playResult.playOption.effectiveFxJudgmentPlayMode(),
			.laserPlayMode = m_playResult.playOption.effectiveLaserJudgmentPlayMode(),
		};
		const FilePathView chartFilePath = args.chartFilePath;
		KscIo::WriteHighScoreInfo(chartFilePath, m_playResult, condition);
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
		{ U"nearCount", U"{:04d}"_fmt(m_playResult.comboStats.near()) },
		{ U"errorCount", U"{:04d}"_fmt(m_playResult.comboStats.error) },
		{ U"gaugePercentageNumber", U"{}"_fmt(static_cast<int32>(m_playResult.gaugePercentage)) },
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

	// StartボタンまたはBackボタンが押されるまで待機
	co_await Co::Any(
		KeyConfig::WaitUntilDown(KeyConfig::kStart),
		KeyConfig::WaitUntilDown(KeyConfig::kBack));

	// 楽曲選択へ戻る
	requestNextScene<SelectScene>();
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
