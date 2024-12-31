#include "play_prepare_scene.hpp"
#include "scene/select/select_scene.hpp"
#include "scene/play/play_scene.hpp"
#include "scene/common/show_loading_one_frame.hpp"

namespace
{
	constexpr Duration kFadeDuration = 1s;

	constexpr Vec2 kJacketPos{ 0, -80 };
	constexpr SizeF kJacketSize{ 300.0, 300.0 };
}

PlayPrepareScene::PlayPrepareScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
	: m_chartFilePath(chartFilePath)
	, m_isAutoPlay(isAutoPlay)
	, m_chartData(kson::LoadKSHChartData(chartFilePath.narrow()))
	, m_jacketTexture(FileSystem::ParentPath(chartFilePath) + Unicode::FromUTF8(m_chartData.meta.jacketFilename))
{
}

Co::Task<void> PlayPrepareScene::start()
{
	m_seStream.play();

	// ジャケットのスケールアニメーション
	const auto runner = Co::Ease(&m_jacketScale, 2s).fromTo(1.2, 1.0).play().runScoped();

	const auto [isWait, isStart, isBack] = co_await Co::Any(
		Co::Delay(2s),
		KeyConfig::WaitUntilDown(KeyConfig::kStart),
		KeyConfig::WaitUntilDown(KeyConfig::kBack));

	if (isBack)
	{
		requestNextScene<SelectScene>();
	}
	else
	{
		requestNextScene<PlayScene>(m_chartFilePath, m_isAutoPlay);
	}
}

void PlayPrepareScene::draw() const
{
	FitToHeight(m_bgTexture).drawAt(Scene::Center());

	const SizeF jacketSize = kJacketSize * m_jacketScale;
	m_jacketTexture.resized(jacketSize).drawAt(Scene::Center().movedBy(Scaled(kJacketPos)));
}

Co::Task<void> PlayPrepareScene::fadeIn()
{
	co_await Co::ScreenFadeIn(kFadeDuration, Palette::White);
}

Co::Task<void> PlayPrepareScene::postFadeOut()
{
	co_await ShowLoadingOneFrame::Play(HasBgYN::No);
}
