#include "result_scene.hpp"
#include "scene/select/select_scene.hpp"
#include "scene/common/show_loading_one_frame.hpp"
#include "high_score/ksc_io.hpp"

namespace
{
	constexpr Duration kFadeDuration = 0.6s;
}

ResultScene::ResultScene(const ResultSceneArgs& args)
	: m_chartData(args.chartData)
	, m_playResult(args.playResult)
	, m_resultPanel(args.chartFilePath, m_chartData, m_playResult)
{
	if (!m_playResult.playOption.isAutoPlay) // オートプレイの場合はスコアを保存しない(オートプレイではリザルト画面を出さないので不要だが一応チェックはする)
	{
		const KscKey condition
		{
			.gaugeType = m_playResult.playOption.gaugeType,
			.turnMode = m_playResult.playOption.turnMode,
			.btPlayMode = m_playResult.playOption.btJudgmentPlayMode(),
			.fxPlayMode = m_playResult.playOption.fxJudgmentPlayMode(),
			.laserPlayMode = m_playResult.playOption.laserJudgmentPlayMode(),
		};
		const FilePathView chartFilePath = args.chartFilePath;
		KscIo::WriteHighScoreInfo(chartFilePath, m_playResult, condition);
	}

	m_bgmStream.play();

	AutoMuteAddon::SetEnabled(true);
}

Co::Task<void> ResultScene::start()
{
	// StartボタンまたはBackボタンが押されるまで待機
	co_await Co::Any(
		KeyConfig::WaitUntilDown(KeyConfig::kStart),
		KeyConfig::WaitUntilDown(KeyConfig::kBack));

	// 楽曲選択へ戻る
	requestNextScene<SelectScene>();
}

void ResultScene::draw() const
{
	FitToHeight(m_bgTexture).drawAt(Scene::Center());

	m_resultPanel.draw();
}

Co::Task<void> ResultScene::fadeIn()
{
	return Co::ScreenFadeIn(kFadeDuration, Palette::White);
}

Co::Task<void> ResultScene::postFadeOut()
{
	// SelectSceneはコンストラクタの処理に時間がかかるので、ローディングはここで出しておく
	return ShowLoadingOneFrame::Play(HasBgYN::Yes);
}
