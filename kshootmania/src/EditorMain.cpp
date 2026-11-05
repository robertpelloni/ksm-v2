#include <Siv3D.hpp>
#include <CoTaskLib.hpp>
#include <NocoUI.hpp>
#include "Common/FrameRateLimit.hpp"
#include "Common/IMEUtils.hpp"
#include "Common/AssetManagement.hpp"
#include "Addon/AutoMuteAddon.hpp"
#include "Addon/CommonSEAddon.hpp"
#include "ksmaudio/ksmaudio.hpp"
#include "RuntimeConfig.hpp"
#include "Input/KeyConfig.hpp"
#include "MusicGame/Graphics/Highway/Highway3DGraphics.hpp"
#include "MusicGame/GameStatus.hpp"
#include "MusicGame/ViewStatus.hpp"
#include "MusicGame/PlayOption.hpp"

#ifdef __APPLE__
#include <ksmplatform_macos/input_method.h>
#endif

class Editor
{
private:
	bool m_exitRequested = false;

	std::shared_ptr<noco::Canvas> m_canvas;
	std::shared_ptr<noco::Canvas> m_editorCanvas;

	Size m_prevSceneSize;

	kson::ChartData m_chartData;
	kson::TimingCache m_timingCache;
	MusicGame::GameStatus m_gameStatus;
	MusicGame::ViewStatus m_viewStatus;
	MusicGame::PlayOption m_playOption;
	MusicGame::Scroll::HighwayScrollContext m_highwayScrollContext;
	std::array<HashSet<kson::Pulse>, kson::kNumLaserLanesSZ> m_laserCurvedPulses;

	std::unique_ptr<MusicGame::Graphics::Highway3DGraphics> m_highwayGraphics;

public:
	Editor()
		: m_canvas(noco::Canvas::Create())
		, m_editorCanvas(noco::Canvas::Create(Scene::Size())->setAutoFitMode(noco::AutoFitMode::MatchSize))
		, m_prevSceneSize(Scene::Size())
		, m_highwayGraphics(std::make_unique<MusicGame::Graphics::Highway3DGraphics>())
	{
		// Editor Base Layout

		// 1. Toolbar (Top)
		auto toolbar = m_editorCanvas->emplaceChild(U"Toolbar", noco::InlineRegion{ .size = Vec2{ Scene::Size().x, 40 }, .sizeRatio = Vec2{ 1.0, 0.0 } });
		toolbar->emplaceComponent<noco::Rect>()->setColor(ColorF(0.1, 0.1, 0.1));

		auto titleLabel = toolbar->emplaceChild(U"TitleLabel", noco::InlineRegion{ .position = Vec2{ 10, 0 }, .sizeRatio = Vec2{ 0.5, 1.0 } });
		titleLabel->emplaceComponent<noco::Label>(U"K-Shoot Editor (Alpha)")
			->setFontSize(20)
			->setColor(Palette::White)
			->setVerticalAlign(noco::VerticalAlign::Middle);

		// 2. Sidebar (Left) - Properties/Tools
		auto sidebar = m_editorCanvas->emplaceChild(U"Sidebar", noco::InlineRegion{ .position = Vec2{ 0, 40 }, .size = Vec2{ 250, Scene::Size().y - 40 }, .sizeRatio = Vec2{ 0.0, 1.0 } });
		sidebar->emplaceComponent<noco::Rect>()->setColor(ColorF(0.15, 0.15, 0.15));

		auto toolLabel = sidebar->emplaceChild(U"ToolLabel", noco::InlineRegion{ .position = Vec2{ 10, 10 }, .size = Vec2{ 230, 30 } });
		toolLabel->emplaceComponent<noco::Label>(U"Tools")
			->setFontSize(24)
			->setColor(Palette::Cyan);

		// Zoom Control
		auto zoomLabel = sidebar->emplaceChild(U"ZoomLabel", noco::InlineRegion{ .position = Vec2{ 10, 60 }, .size = Vec2{ 100, 30 } });
		zoomLabel->emplaceComponent<noco::Label>(U"Lane Zoom:")->setFontSize(16);

		auto zoomInBtn = sidebar->emplaceChild(U"ZoomInBtn", noco::InlineRegion{ .position = Vec2{ 120, 60 }, .size = Vec2{ 30, 30 } });
		zoomInBtn->emplaceComponent<noco::Rect>()->setColor(ColorF(0.3));
		zoomInBtn->emplaceComponent<noco::Label>(U"+")->setHorizontalAlign(noco::HorizontalAlign::Center);
		zoomInBtn->setInteractable(true);

		auto zoomOutBtn = sidebar->emplaceChild(U"ZoomOutBtn", noco::InlineRegion{ .position = Vec2{ 160, 60 }, .size = Vec2{ 30, 30 } });
		zoomOutBtn->emplaceComponent<noco::Rect>()->setColor(ColorF(0.3));
		zoomOutBtn->emplaceComponent<noco::Label>(U"-")->setHorizontalAlign(noco::HorizontalAlign::Center);
		zoomOutBtn->setInteractable(true);

		// Split Control
		auto splitLabel = sidebar->emplaceChild(U"SplitLabel", noco::InlineRegion{ .position = Vec2{ 10, 110 }, .size = Vec2{ 100, 30 } });
		splitLabel->emplaceComponent<noco::Label>(U"Lane Split:")->setFontSize(16);

		auto splitBtn = sidebar->emplaceChild(U"SplitBtn", noco::InlineRegion{ .position = Vec2{ 120, 110 }, .size = Vec2{ 70, 30 } });
		splitBtn->emplaceComponent<noco::Rect>()->setColor(ColorF(0.3));
		splitBtn->emplaceComponent<noco::Label>(U"Toggle")->setHorizontalAlign(noco::HorizontalAlign::Center);
		splitBtn->setInteractable(true);

		// Event handlers for dynamic view modifications
		zoomInBtn->addEventHandler(noco::Event::Type::Click, [this](const noco::Event& e) {
			m_viewStatus.camStatus.zoomBottom += 10.0;
		});

		zoomOutBtn->addEventHandler(noco::Event::Type::Click, [this](const noco::Event& e) {
			m_viewStatus.camStatus.zoomBottom -= 10.0;
		});

		splitBtn->addEventHandler(noco::Event::Type::Click, [this](const noco::Event& e) {
			m_viewStatus.camStatus.centerSplit = (m_viewStatus.camStatus.centerSplit > 0.0) ? 0.0 : 50.0;
		});

		// Effects List UI
		auto fxLabel = sidebar->emplaceChild(U"FXLabel", noco::InlineRegion{ .position = Vec2{ 10, 160 }, .size = Vec2{ 230, 30 } });
		fxLabel->emplaceComponent<noco::Label>(U"Audio FX:")->setFontSize(20)->setColor(Palette::Cyan);

		auto fxListContainer = sidebar->emplaceChild(U"FXList", noco::InlineRegion{ .position = Vec2{ 10, 190 }, .size = Vec2{ 230, 150 } });
		// We will populate this container dynamically when a chart is loaded

		// 3. Chart Canvas Area (Center/Right)
		auto chartArea = m_editorCanvas->emplaceChild(U"ChartArea", noco::InlineRegion{ .position = Vec2{ 250, 40 }, .size = Vec2{ Scene::Size().x - 250, Scene::Size().y - 40 }, .sizeRatio = Vec2{ 1.0, 1.0 } });
		chartArea->emplaceComponent<noco::Rect>()->setColor(ColorF(0.05, 0.05, 0.05));
	}

	void update()
	{
		m_editorCanvas->update();
		m_canvas->update();

		// Basic interaction: Left click to advance the preview scroll forward.
		// For proper implementation, a timeline scrub bar should be introduced.
		if (MouseL.down())
		{
			// Advance pulse for preview
			m_gameStatus.currentPulse += kson::kResolution;
			m_gameStatus.currentPulseDouble += kson::kResolution;
			m_gameStatus.currentTimeSec += 0.5; // Simulate advancing in time slightly
		}

		if (MouseR.down())
		{
			if (m_gameStatus.currentPulse >= kson::kResolution)
			{
				m_gameStatus.currentPulse -= kson::kResolution;
				m_gameStatus.currentPulseDouble -= kson::kResolution;
				m_gameStatus.currentTimeSec -= 0.5;
			}
		}

		const auto sceneSize = Scene::Size();
		if (m_prevSceneSize != sceneSize)
		{
			m_prevSceneSize = sceneSize;
		}

		// ショートカットキー
		const bool isWindowActive = Window::GetState().focused;
		if (isWindowActive)
		{
			const bool ctrl = noco::detail::KeyCommandControl.pressed();
			const bool alt = KeyAlt.pressed();
			const bool shift = KeyShift.pressed();

#ifdef __APPLE__
			// macOSはCommand+Qで終了
			if (ctrl && !alt && !shift && KeyQ.down())
			{
				requestExit();
			}
#else
			// Windows/LinuxはCtrl+Wで終了
			if (ctrl && !alt && !shift && KeyW.down())
			{
				requestExit();
			}
#endif
		}
	}

	void draw() const
	{
		// Draw 3D Highway Preview first (so UI sits on top)
		// Fake view status
		MusicGame::ViewStatus viewStatus = m_viewStatus;
		m_highwayGraphics->update(viewStatus);

		// Render the highway 2D backing to texture
		m_highwayGraphics->draw2D(m_chartData, m_laserCurvedPulses, m_playOption, m_timingCache, m_gameStatus, viewStatus, m_highwayScrollContext);

		// Combine to 3D Viewport
		const BasicCamera3D camera{ viewStatus.camStatus.cameraPos, viewStatus.camStatus.cameraLookAt, Vec3::Up() };
		Graphics3D::SetCameraTransform(camera);
		m_highwayGraphics->draw3D(viewStatus);

		m_canvas->draw();
		m_editorCanvas->draw();
	}

	[[nodiscard]]
	bool isExitRequested() const
	{
		return m_exitRequested;
	}

	void loadChart(FilePathView path)
	{
		if (path.ends_with(U".kson"))
		{
			auto loaded = kson::LoadKsonChartData(path.toUTF8());
			if (loaded.error == kson::Error::None)
			{
				m_chartData = std::move(loaded.chartData);
			}
		}
		else if (path.ends_with(U".ksh"))
		{
			auto loaded = kson::LoadKshChartData(path.toUTF8());
			if (loaded.error == kson::Error::None)
			{
				m_chartData = std::move(loaded.chartData);
			}
		}

		// Ensure stop events are baked into scroll speed for visual simulation
		m_chartData.beat.scrollSpeed = kson::ExpandCurveSegments(m_chartData.beat.scrollSpeed, kson::kCurveSubdivisionInterval);
		m_chartData.beat.scrollSpeed = kson::BakeStopIntoScrollSpeed(m_chartData.beat.scrollSpeed, m_chartData.beat.stop);

		m_timingCache = kson::CreateTimingCache(m_chartData.beat);

		updateFXListUI();
	}

	void updateFXListUI()
	{
		if (!m_editorCanvas) return;
		auto sidebar = m_editorCanvas->findByName(U"Sidebar");
		if (!sidebar) return;
		auto fxList = sidebar->findByName(U"FXList");
		if (!fxList) return;

		fxList->destroyAllChildren(); // Clear previous list

		int32 yOffset = 0;
		const auto& audioEffect = m_chartData.audio.audioEffect;

		// 1. Render #define_fx
		for (const auto& [name, def] : audioEffect.fx.def)
		{
			auto item = fxList->emplaceChild(U"FXItem_" + Unicode::FromUTF8(name), noco::InlineRegion{ .position = Vec2{ 0, yOffset }, .size = Vec2{ 230, 20 } });
			String text = U"[FX] " + Unicode::FromUTF8(name) + U" (" + Unicode::FromUTF8(kson::AudioEffectTypeToStr(def.type)) + U")";
			item->emplaceComponent<noco::Label>(text)->setFontSize(14)->setColor(Palette::Lightgreen);
			yOffset += 25;
		}

		// 2. Render #define_filter
		for (const auto& [name, def] : audioEffect.laser.def)
		{
			auto item = fxList->emplaceChild(U"FilterItem_" + Unicode::FromUTF8(name), noco::InlineRegion{ .position = Vec2{ 0, yOffset }, .size = Vec2{ 230, 20 } });
			String text = U"[Filter] " + Unicode::FromUTF8(name) + U" (" + Unicode::FromUTF8(kson::AudioEffectTypeToStr(def.type)) + U")";
			item->emplaceComponent<noco::Label>(text)->setFontSize(14)->setColor(Palette::Lightpink);
			yOffset += 25;
		}
	}

	void requestExit()
	{
		m_exitRequested = true;
	}
};

void CreateAppDataDirectory()
{
#ifdef __APPLE__
	const FilePath appDataDir = FsUtils::AppDataDirectoryPath();
	Logger << U"[ksmeditor info] AppDataDirectory path: " << appDataDir;

	if (!FileSystem::Exists(appDataDir))
	{
		Logger << U"[ksmeditor info] Creating AppDataDirectory...";
		FileSystem::CreateDirectories(appDataDir);
		const bool created = FileSystem::Exists(appDataDir);
		Logger << U"[ksmeditor info] Created: " << created;
	}
	else
	{
		Logger << U"[ksmeditor info] AppDataDirectory already exists";
	}
#endif
}

void EditorMain()
{
	// Escキーによるプログラム終了を無効化
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	// F1キーによるライセンス表示を無効化
	LicenseManager::DisableDefaultTrigger();

	Window::SetTitle(U"K-Shoot Editor");
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(800, 600);

	// カレントディレクトリを設定(ここ以外で変更しないこと)
	FileSystem::ChangeCurrentDirectory(FsUtils::ResourceDirectoryPath());

	// デフォルト色を指定
	Scene::SetBackground(Color{ 51, 51, 77 });
	Graphics3D::SetGlobalAmbientColor(Palette::White);
	Graphics3D::SetSunColor(Palette::Black);

	// 音声処理のバックエンドを初期化
#ifdef _WIN32
	ksmaudio::Init(s3d::Platform::Windows::Window::GetHWND());
#else
	ksmaudio::Init(nullptr);
#endif

	// アプリケーションデータディレクトリを作成(macOSのみ)
	CreateAppDataDirectory();

	// config.iniを読み込み
	ConfigIni::Load();

	// 言語ファイルを読み込み
	I18n::LoadLanguage(ConfigIni::GetString(ConfigIni::Key::kLanguage));

	// アセット一覧を登録
	AssetManagement::RegisterAssets();

	// フレームレート制限
	Graphics::SetVSyncEnabled(false);
	Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimit>(300), -100);

	// 毎フレーム連続してアセット生成した時の警告を無効化
	Profiler::EnableAssetCreationWarning(false);

#ifdef _DEBUG
	// ライブラリ側のデバッグ用にコンソール表示(Debugビルドの場合のみ)
	Console.open();
#endif

	// ライブラリ初期化
	Co::Init();
	noco::Init();

	// NocoUIのグローバルデフォルトフォントを設定
	const Font uiFont = Font(FontMethod::MSDF, 36, Typeface::Regular);
	noco::SetGlobalDefaultFont(uiFont);

	Editor editor;

	// Note: Currently we don't have a file picker in NocoUI scaffolded yet.
	// For testing chart rendering during layout init, we attempt to load a test file if present.
	if (FileSystem::Exists(U"test.kson"))
	{
		editor.loadChart(U"test.kson");
	}
	else if (FileSystem::Exists(U"test.ksh"))
	{
		editor.loadChart(U"test.ksh");
	}

	while (System::Update())
	{
		editor.update();
		if (editor.isExitRequested())
		{
			break;
		}
		editor.draw();
	}

	// config.iniを保存
	ConfigIni::Save();

	// 音声のバックエンドを終了
	ksmaudio::Terminate();
}

void Main()
{
	try
	{
		EditorMain();
	}
	catch (const Error& e)
	{
		System::MessageBoxOK(e.what(), MessageBoxStyle::Error);
		throw;
	}
}
