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

public:
	Editor()
		: m_canvas(noco::Canvas::Create())
		, m_editorCanvas(noco::Canvas::Create(Scene::Size())->setAutoFitMode(noco::AutoFitMode::MatchSize))
		, m_prevSceneSize(Scene::Size())
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

		// 3. Chart Canvas Area (Center/Right)
		auto chartArea = m_editorCanvas->emplaceChild(U"ChartArea", noco::InlineRegion{ .position = Vec2{ 250, 40 }, .size = Vec2{ Scene::Size().x - 250, Scene::Size().y - 40 }, .sizeRatio = Vec2{ 1.0, 1.0 } });
		chartArea->emplaceComponent<noco::Rect>()->setColor(ColorF(0.05, 0.05, 0.05));

		auto placeholderLabel = chartArea->emplaceChild(U"Placeholder", noco::InlineRegion{ .sizeRatio = Vec2::One() });
		placeholderLabel->emplaceComponent<noco::Label>(U"Chart editing canvas goes here")
			->setFontSize(28)
			->setColor(Palette::Gray)
			->setHorizontalAlign(noco::HorizontalAlign::Center)
			->setVerticalAlign(noco::VerticalAlign::Middle);
	}

	void update()
	{
		m_editorCanvas->update();
		m_canvas->update();

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
		m_canvas->draw();
		m_editorCanvas->draw();
	}

	[[nodiscard]]
	bool isExitRequested() const
	{
		return m_exitRequested;
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
