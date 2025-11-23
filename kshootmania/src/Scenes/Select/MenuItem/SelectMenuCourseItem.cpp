#include "SelectMenuCourseItem.hpp"
#include "Course/CoursePlayState.hpp"
#include "I18n/I18n.hpp"
#include "RuntimeConfig.hpp"
#include "HighScore/KscKey.hpp"
#include "HighScore/KscIO.hpp"
#include "NocoExtensions/NocoUtils.hpp"
#include "NocoExtensions/VerticalMarquee.hpp"

namespace
{
	// TODO: PlayOptionを選曲画面側で生成してそれを使うように
	KscKey CreateKscKeyFromConfig()
	{
		return KscKey
		{
			.gaugeType = RuntimeConfig::GetGaugeType(),
			.turnMode = RuntimeConfig::GetTurnMode(),
			.btPlayMode = RuntimeConfig::GetJudgmentPlayModeBT(),
			.fxPlayMode = RuntimeConfig::GetJudgmentPlayModeFX(),
			.laserPlayMode = RuntimeConfig::GetJudgmentPlayModeLaser(),
		};
	}
}

SelectMenuCourseItem::SelectMenuCourseItem(const CourseInfo& courseInfo)
	: m_courseInfo(courseInfo)
	, m_highScoreInfo(KscIO::ReadCourseHighScoreInfo(courseInfo.filePath, CreateKscKeyFromConfig()))
{
}

void SelectMenuCourseItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	// コースの全譜面が存在するかチェック
	if (!m_courseInfo.isValid())
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorSomeChartMissingInCourse), MessageBoxStyle::Error);
		return;
	}

	// コースプレイ開始
	const FilePath firstChartPath = m_courseInfo.charts[0].absolutePath;
	if (!FileSystem::Exists(firstChartPath))
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorChartFileNotFound), MessageBoxStyle::Error);
		return;
	}

	// コース状態を初期化してPlaySceneに渡す
	const KscKey kscKey = CreateKscKeyFromConfig();
	CoursePlayState courseState{ m_courseInfo, kscKey };
	context.fnMoveToPlayScene(firstChartPath, MusicGame::IsAutoPlayYN::No, courseState);
}

void SelectMenuCourseItem::decideAutoPlay(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	// コースの全譜面が存在するかチェック
	if (!m_courseInfo.isValid())
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorSomeChartMissingInCourse), MessageBoxStyle::Error);
		return;
	}

	// オートプレイでコースプレイ開始
	const FilePath firstChartPath = m_courseInfo.charts[0].absolutePath;
	if (!FileSystem::Exists(firstChartPath))
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorChartFileNotFound), MessageBoxStyle::Error);
		return;
	}

	// コース状態を初期化してPlaySceneに渡す
	const KscKey kscKey = CreateKscKeyFromConfig();
	CoursePlayState courseState{ m_courseInfo, kscKey };
	context.fnMoveToPlayScene(firstChartPath, MusicGame::IsAutoPlayYN::Yes, courseState);
}

void SelectMenuCourseItem::setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isSong", false },
		{ U"center_isDirectory", false },
		{ U"center_isSubDirectory", false },
		{ U"center_isCourse", true },
	});

	// 難易度は非表示
	for (int32 i = 0; i < kNumDifficulties; ++i)
	{
		canvas.setParamValues({
			{ U"center_difficulty{}Enabled"_fmt(i), false },
			{ U"center_difficulty{}LevelIndex"_fmt(i), -1 },
		});
	}

	// コースのハイスコア情報を取得
	const KscKey kscKey = CreateKscKeyFromConfig();
	const int32 medalIndex = static_cast<int32>(m_highScoreInfo.medal());
	const int32 achievementRate = m_highScoreInfo.percent(kscKey.gaugeType);

	// コース情報を設定
	canvas.setParamValues({
		{ U"center_title", m_courseInfo.title },
		{ U"center_artist", U"COURSE ({} charts)"_fmt(m_courseInfo.chartCount()) },
		{ U"center_bpm", U"" },
		{ U"center_jacketAuthor", U"" },
		{ U"center_information", m_courseInfo.information },
		{ U"center_chartAuthor", U"" },
		{ U"center_difficultyCursorState", U"difficulty0" },
		{ U"center_medalIndex", medalIndex },
		{ U"center_highScoreGradeIndex", -1 },
		{ U"center_highScore", U"" },
		{ U"center_gaugePercentage", ToString(achievementRate) },
	});

	// アイコン画像を設定
	// TODO: タグ等で取得可能にする
	if (const auto iconNode = NocoUtils::GetNodeByPath(&canvas, { U"CenterItem", U"Course", U"Icon" }))
	{
		if (m_courseInfo.iconPath.isEmpty())
		{
			// アイコンが指定されていない場合は非表示
			iconNode->setActive(false);
		}
		else
		{
			// アイコンが指定されている場合はテクスチャロードして表示
			const Texture iconTexture = context.fnGetIconTexture(m_courseInfo.iconPath);
			iconNode->setActive(!iconTexture.isEmpty());
			if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { U"CenterItem", U"Course", U"Icon" }))
			{
				sprite->setTexture(iconTexture);
			}
		}
	}

	// コースの各曲をSubCanvasノードとして追加
	if (const auto chartItemRoot = NocoUtils::GetNodeByPath(&canvas, { U"CenterItem", U"Course", U"ChartItemRoot" }))
	{
		chartItemRoot->removeChildrenAll();
		chartItemRoot->removeComponentsAll<noco::VerticalMarquee>(noco::RecursiveYN::No);

		// 各曲のSubCanvasノードを追加
		for (size_t i = 0; i < m_courseInfo.charts.size(); ++i)
		{
			const auto& chart = m_courseInfo.charts[i];

			String songTitle = U"---";
			String artistName = U"---";
			int32 chartDifficultyIdx = 0;
			int32 levelIdx = 0;

			if (FileSystem::Exists(chart.absolutePath))
			{
				const kson::MetaChartData chartData = kson::LoadKSHMetaChartData(chart.absolutePath.narrow());
				if (chartData.error == kson::ErrorType::None)
				{
					songTitle = Unicode::FromUTF8(chartData.meta.title);
					artistName = Unicode::FromUTF8(chartData.meta.artist);
					chartDifficultyIdx = chartData.meta.difficulty.idx;
					levelIdx = chartData.meta.level - 1;
				}
			}

			const auto& node = chartItemRoot->addSubCanvasNodeAsChild(
				U"ui/parts/select_course_chart_item.noco",
				{
					{ U"songTitle", songTitle },
					{ U"artistName", artistName },
					{ U"difficultyIndex", chartDifficultyIdx },
					{ U"levelIndex", levelIdx },
				});

			if (const auto subCanvas = node->getComponent<noco::SubCanvas>())
			{
				if (auto itemCanvas = subCanvas->canvas())
				{
					if (const auto jacketSprite = NocoUtils::GetComponentByPath<noco::Sprite>(itemCanvas.get(), { U"SelectCourseChartItem", U"Jacket" }))
					{
						if (FileSystem::Exists(chart.absolutePath))
						{
							const kson::MetaChartData chartData = kson::LoadKSHMetaChartData(chart.absolutePath.narrow());
							if (chartData.error == kson::ErrorType::None && !chartData.meta.jacketFilename.empty())
							{
								const FilePath jacketPath = FileSystem::PathAppend(
									FileSystem::ParentPath(chart.absolutePath),
									Unicode::FromUTF8(chartData.meta.jacketFilename));
								const Texture jacketTexture = context.fnGetJacketTexture(jacketPath);
								jacketSprite->setTexture(jacketTexture);
								if (jacketTexture.isEmpty())
								{
									jacketSprite->setColor(ColorF{ 0.0, 0.0 });
								}
								else
								{
									jacketSprite->setColor(Palette::White);
								}
							}
							else
							{
								jacketSprite->setColor(ColorF{ 0.0, 0.0 });
							}
						}
						else
						{
							jacketSprite->setColor(ColorF{ 0.0, 0.0 });
						}
					}
				}
			}
		}

		chartItemRoot->setVerticalScrollable(true);
		chartItemRoot->setScrollBarType(noco::ScrollBarType::Hidden);
		chartItemRoot->emplaceComponent<noco::VerticalMarquee>(0.75s, 0.75s, 64.0);
	}
}

void SelectMenuCourseItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const
{
	// コースのハイスコア情報を取得
	const KscKey kscKey = CreateKscKeyFromConfig();
	const int32 medalIndex = static_cast<int32>(m_highScoreInfo.medal());
	const int32 achievementRate = m_highScoreInfo.percent(kscKey.gaugeType);

	canvas.setParamValues({
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isCourse", true },
		{ paramNamePrefix + U"title", m_courseInfo.title },
		{ paramNamePrefix + U"artist", U"" },
		{ paramNamePrefix + U"levelIndex", -1 },
		{ paramNamePrefix + U"medalIndex", medalIndex },
		{ paramNamePrefix + U"highScoreGradeIndex", -1 },
		{ paramNamePrefix + U"gaugePercentage", ToString(achievementRate) },
	});

	// アイコン画像を設定
	// TODO: タグ等で取得可能にする
	if (const auto iconNode = NocoUtils::GetNodeByPath(&canvas, { nodeName.data(), U"Course", U"Icon" }))
	{
		if (m_courseInfo.iconPath.isEmpty())
		{
			// アイコンが指定されていない場合は非表示
			iconNode->setActive(false);
		}
		else
		{
			// アイコンが指定されている場合はテクスチャロードして表示
			const Texture iconTexture = context.fnGetIconTexture(m_courseInfo.iconPath);
			iconNode->setActive(!iconTexture.isEmpty());
			if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { nodeName.data(), U"Course", U"Icon" }))
			{
				sprite->setTexture(iconTexture);
			}
		}
	}
}

void SelectMenuCourseItem::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	// コースファイルをエクスプローラで開く
	System::ShowInFileManager(m_courseInfo.filePath);
}

Optional<HighScoreInfo> SelectMenuCourseItem::highScoreInfo([[maybe_unused]] int32 difficultyIdx) const
{
	return m_highScoreInfo;
}
