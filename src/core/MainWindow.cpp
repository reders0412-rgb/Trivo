#include "MainWindow.h"
#include "Viewport.h"
#include "Scene.h"
#include "../ui/Sidebar.h"
#include "../ui/ToolBar.h"
#include "../ui/InfoPanel.h"
#include "../ui/AboutDialog.h"
#include "../ui/DropOverlay.h"
#include "../utils/FileUtils.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QFileDialog>
#include <QSettings>
#include <QColorDialog>
#include <QMimeData>
#include <QUrl>
#include <QScreen>
#include <QPixmap>
#include <QDateTime>
#include <QStandardPaths>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QDesktopServices>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QScrollArea>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_scene(std::make_shared<Scene>())
{
    setWindowTitle("Trivo");
    setMinimumSize(1100, 700);
    setAcceptDrops(true);

    loadSettings();
    setupUI();
    setupMenuBar();
    applyTheme(m_darkTheme);
}

MainWindow::~MainWindow() { saveSettings(); }

// ── Setup ──────────────────────────────────────────────────────────────────────

void MainWindow::setupUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    m_toolbar = new ToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_toolbar);

    m_viewport = new Viewport(m_scene, central);
    m_viewport->setMinimumWidth(600);

    m_sidebar = new Sidebar(m_scene, central);
    m_sidebar->setFixedWidth(310);

    auto *splitter = new QSplitter(Qt::Horizontal, central);
    splitter->addWidget(m_viewport);
    splitter->addWidget(m_sidebar);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setHandleWidth(2);

    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(splitter);

    m_dropOverlay = new DropOverlay(this);
    m_dropOverlay->hide();

    m_infoPanel = new InfoPanel(this);
    statusBar()->addPermanentWidget(m_infoPanel, 1);

    // ── Toolbar connections ────────────────────────────────────────────────────
    connect(m_toolbar, &ToolBar::openFileRequested,     this, &MainWindow::onOpenFile);
    connect(m_toolbar, &ToolBar::addFileRequested,      this, &MainWindow::onAddFile);
    connect(m_toolbar, &ToolBar::clearSceneRequested,   this, &MainWindow::onClearScene);
    connect(m_toolbar, &ToolBar::themeToggled,          this, &MainWindow::onToggleTheme);
    connect(m_toolbar, &ToolBar::resetCameraRequested,  this, &MainWindow::onResetCamera);
    connect(m_toolbar, &ToolBar::screenshotRequested,   this, &MainWindow::onScreenshot);
    connect(m_toolbar, &ToolBar::animationToggled,      this, &MainWindow::onToggleAnimation);
    connect(m_toolbar, &ToolBar::animationSpeedChanged, this, &MainWindow::onAnimationSpeedChanged);
    connect(m_toolbar, &ToolBar::gizmoModeChanged,      this, &MainWindow::onGizmoModeChanged);
    connect(m_toolbar, &ToolBar::textureVisibleChanged, this, &MainWindow::onTextureVisibleChanged);

    // ── Sidebar connections ───────────────────────────────────────────────────
    connect(m_sidebar, &Sidebar::backgroundColorChangeRequested,
            this, &MainWindow::onBackgroundColorChanged);
    connect(m_sidebar, &Sidebar::lightPresetChanged,
            this, &MainWindow::onLightPresetChanged);
    connect(m_sidebar, &Sidebar::aboutRequested, this, &MainWindow::onAbout);

    // ── Viewport → toolbar gizmo sync (키보드로 바꿨을 때) ───────────────────
    connect(m_viewport, &Viewport::gizmoModeChangedByKey, this, [this](int mode){
        m_toolbar->setGizmoMode(mode);
    });
    connect(m_viewport, &Viewport::modelSelected, this, &MainWindow::onModelSelected);

    // ── Scene signals ─────────────────────────────────────────────────────────
    connect(m_scene.get(), &Scene::modelAdded, this, [this](std::shared_ptr<SceneModel>){
        m_sidebar->refresh();
    });
    connect(m_scene.get(), &Scene::modelAdded,   m_infoPanel, &InfoPanel::refresh);
    connect(m_scene.get(), &Scene::sceneCleared, m_infoPanel, &InfoPanel::clear);
    connect(m_scene.get(), &Scene::sceneCleared, m_sidebar,   &Sidebar::refresh);
    connect(m_scene.get(), &Scene::selectionChanged, this, [this](int idx){
        if (idx >= 0 && idx < (int)m_scene->models().size())
            m_infoPanel->refresh(m_scene->models()[idx]);
        else
            m_infoPanel->clear();
    });
}

void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu(tr("파일 / File"));
    fileMenu->addAction(tr("열기…"),          this, &MainWindow::onOpenFile, QKeySequence::Open);
    fileMenu->addAction(tr("씬에 추가…"),     this, &MainWindow::onAddFile,  QKeySequence("Ctrl+Shift+O"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("스크린샷 저장"),  this, &MainWindow::onScreenshot, QKeySequence("Ctrl+P"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("🔗 기본 3D 뷰어로 설정 / Register as Default Viewer"),
                        this, &MainWindow::onRegisterFileAssociation);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("종료"), this, &QMainWindow::close, QKeySequence::Quit);

    auto *viewMenu = menuBar()->addMenu(tr("보기 / View"));
    viewMenu->addAction(tr("카메라 초기화"), this, &MainWindow::onResetCamera, QKeySequence("R"));
    viewMenu->addAction(tr("씬 비우기"),    this, &MainWindow::onClearScene);
    viewMenu->addSeparator();
    viewMenu->addAction(tr("다크/라이트 전환"), this, &MainWindow::onToggleTheme, QKeySequence("Ctrl+T"));

    auto *helpMenu = menuBar()->addMenu(tr("도움말 / Help"));
    helpMenu->addAction(tr("Trivo 정보…"), this, &MainWindow::onAbout);
}

// ── Theme ──────────────────────────────────────────────────────────────────────

void MainWindow::applyTheme(bool dark)
{
    m_darkTheme = dark;
    const QString qss = dark ? R"(
        QMainWindow, QWidget { background: #1a1a1f; color: #e8e8ec; }
        QMenuBar { background: #141418; color: #e8e8ec; border-bottom: 1px solid #2a2a32; }
        QMenuBar::item:selected { background: #2d2d38; }
        QMenu { background: #1f1f28; color: #e8e8ec; border: 1px solid #2a2a32; }
        QMenu::item:selected { background: #3a7bd5; }
        QToolBar { background: #141418; border-bottom: 1px solid #2a2a32; spacing: 4px; padding: 4px; }
        QToolButton { background: transparent; color: #e8e8ec; border-radius: 6px; padding: 5px 9px; }
        QToolButton:hover { background: #2d2d38; }
        QToolButton:pressed { background: #3a7bd5; }
        QToolButton:checked { background: #1e3a6e; border: 1px solid #3a7bd5; }
        QSplitter::handle { background: #2a2a32; }
        QStatusBar { background: #141418; color: #888; border-top: 1px solid #2a2a32; }
        QScrollBar:vertical { background: #1a1a1f; width: 8px; }
        QScrollBar::handle:vertical { background: #3a3a48; border-radius: 4px; min-height: 20px; }
        QScrollBar:horizontal { background: #1a1a1f; height: 8px; }
        QScrollBar::handle:horizontal { background: #3a3a48; border-radius: 4px; }
        QLabel { color: #e8e8ec; }
        QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 6px 14px; }
        QPushButton:hover { background: #3a7bd5; border-color: #3a7bd5; }
        QPushButton:pressed { background: #2d6bc4; }
        QComboBox { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 4px 8px; }
        QComboBox::drop-down { border: none; }
        QComboBox QAbstractItemView { background: #1f1f28; color: #e8e8ec; selection-background-color: #3a7bd5; }
        QSlider::groove:horizontal { background: #2d2d38; height: 4px; border-radius: 2px; }
        QSlider::handle:horizontal { background: #3a7bd5; width: 14px; height: 14px; border-radius: 7px; margin: -5px 0; }
        QTreeWidget { background: #1a1a1f; color: #e8e8ec; border: none; }
        QTreeWidget::item:hover { background: #2d2d38; }
        QTreeWidget::item:selected { background: #3a7bd5; }
        QGroupBox { color: #aaa; border: 1px solid #2a2a32; border-radius: 8px; margin-top: 8px; padding-top: 8px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #888; font-size: 10px; }
        QSpinBox, QDoubleSpinBox { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 3px 6px; }
        QCheckBox { color: #e8e8ec; }
        QFrame[frameShape="5"] { border: 1px solid #2a2a32; border-radius: 6px; background: #141418; }
        QTabWidget::pane { border: 1px solid #2a2a32; }
        QTabBar::tab { background: #1a1a1f; color: #e8e8ec; border: 1px solid #2a2a32; border-bottom: none; padding: 5px 10px; margin-right: 2px; border-radius: 4px 4px 0 0; }
        QTabBar::tab:selected { background: #2d2d38; }
        QTabBar::tab:hover { background: #232330; }
        QDialog { background: #1a1a1f; color: #e8e8ec; }
    )" : R"(
        QMainWindow, QWidget { background: #f5f5f8; color: #1a1a2e; }
        QMenuBar { background: #ffffff; color: #1a1a2e; border-bottom: 1px solid #ddd; }
        QMenuBar::item:selected { background: #e8f0fe; }
        QMenu { background: #ffffff; color: #1a1a2e; border: 1px solid #ddd; }
        QMenu::item:selected { background: #3a7bd5; color: white; }
        QToolBar { background: #ffffff; border-bottom: 1px solid #ddd; spacing: 4px; padding: 4px; }
        QToolButton { background: transparent; color: #1a1a2e; border-radius: 6px; padding: 5px 9px; }
        QToolButton:hover { background: #e8f0fe; }
        QToolButton:pressed { background: #3a7bd5; color: white; }
        QToolButton:checked { background: #c5d8f8; border: 1px solid #3a7bd5; }
        QSplitter::handle { background: #ddd; }
        QStatusBar { background: #ffffff; color: #666; border-top: 1px solid #ddd; }
        QScrollBar:vertical { background: #f5f5f8; width: 8px; }
        QScrollBar::handle:vertical { background: #ccc; border-radius: 4px; min-height: 20px; }
        QLabel { color: #1a1a2e; }
        QPushButton { background: #e8f0fe; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 6px 14px; }
        QPushButton:hover { background: #3a7bd5; color: white; border-color: #3a7bd5; }
        QComboBox { background: #ffffff; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 4px 8px; }
        QTreeWidget { background: #ffffff; color: #1a1a2e; border: none; }
        QTreeWidget::item:hover { background: #e8f0fe; }
        QTreeWidget::item:selected { background: #3a7bd5; color: white; }
        QGroupBox { color: #555; border: 1px solid #ddd; border-radius: 8px; margin-top: 8px; padding-top: 8px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #666; font-size: 10px; }
        QSlider::groove:horizontal { background: #ddd; height: 4px; border-radius: 2px; }
        QSlider::handle:horizontal { background: #3a7bd5; width: 14px; height: 14px; border-radius: 7px; margin: -5px 0; }
        QSpinBox, QDoubleSpinBox { background: #ffffff; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 3px 6px; }
        QCheckBox { color: #1a1a2e; }
        QFrame[frameShape="5"] { border: 1px solid #ddd; border-radius: 6px; background: #ffffff; }
        QTabWidget::pane { border: 1px solid #ddd; }
        QTabBar::tab { background: #f0f0f4; color: #1a1a2e; border: 1px solid #ddd; border-bottom: none; padding: 5px 10px; margin-right: 2px; border-radius: 4px 4px 0 0; }
        QTabBar::tab:selected { background: #ffffff; }
        QTabBar::tab:hover { background: #e8f0fe; }
        QDialog { background: #f5f5f8; color: #1a1a2e; }
    )";

    static_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(qss);
    if (m_toolbar) m_toolbar->updateThemeIcon(dark);
}

// ── File handling ──────────────────────────────────────────────────────────────

void MainWindow::openFile(const QString &path)
{
    m_scene->clear();
    m_scene->addModel(path);
    m_viewport->resetCamera();
    setWindowTitle(QString("Trivo — %1").arg(QFileInfo(path).fileName()));
}

void MainWindow::onOpenFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("3D 파일 열기 / Open 3D File"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        FileUtils::filterString());
    if (!path.isEmpty()) openFile(path);
}

void MainWindow::onAddFile()
{
    const QStringList paths = QFileDialog::getOpenFileNames(
        this, tr("씬에 모델 추가 / Add Models to Scene"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        FileUtils::filterString());
    for (const QString &p : paths) m_scene->addModel(p);
}

void MainWindow::onClearScene() { m_scene->clear(); setWindowTitle("Trivo"); }

// ── Drag & Drop ───────────────────────────────────────────────────────────────

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        for (const QUrl &u : e->mimeData()->urls())
            if (FileUtils::isSupported(u.toLocalFile())) {
                e->acceptProposedAction();
                if (m_dropOverlay) m_dropOverlay->show();
                return;
            }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if (m_dropOverlay) m_dropOverlay->hide();
    bool first = true;
    for (const QUrl &u : e->mimeData()->urls()) {
        const QString path = u.toLocalFile();
        if (FileUtils::isSupported(path)) {
            if (first) { m_scene->clear(); first = false; }
            m_scene->addModel(path);
        }
    }
    if (!first) m_viewport->resetCamera();
}

// ── Actions ───────────────────────────────────────────────────────────────────

void MainWindow::onToggleTheme()                  { applyTheme(!m_darkTheme); }
void MainWindow::onResetCamera()                  { m_viewport->resetCamera(); }
void MainWindow::onToggleAnimation()              { m_viewport->toggleAnimation(); }
void MainWindow::onAnimationSpeedChanged(double s){ m_viewport->setAnimationSpeed(s); }
void MainWindow::onLightPresetChanged(int i)      { m_viewport->setLightPreset(i); }

void MainWindow::onGizmoModeChanged(int mode)
{
    m_viewport->setGizmoMode(static_cast<GizmoMode>(mode));
}

void MainWindow::onTextureVisibleChanged(bool v)
{
    m_scene->setTextureVisible(v);
}

void MainWindow::onModelSelected(int index)
{
    m_sidebar->syncSelection(index);
}

void MainWindow::onAbout()
{
    AboutDialog dlg(m_darkTheme, this);
    dlg.exec();
}

// ── Screenshot + 미리보기 다이얼로그 ──────────────────────────────────────────────

void MainWindow::onScreenshot()
{
    // 1. 저장 폴더 준비
    const QString docPath   = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString targetDir = docPath + "/trivo";
    QDir().mkpath(targetDir);

    const QString ts  = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString out = targetDir + "/trivo_" + ts + ".png";

    // 2. 씬 뷰만 캡처 (깜빡임 없이)
    m_viewport->takeScreenshot(out);

    // 3. 미리보기 다이얼로그
    auto *dlg = new QDialog(this, Qt::Dialog);
    dlg->setWindowTitle("📷 스크린샷 저장됨 / Screenshot Saved");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(false);   // 비모달 → 바로 사용 가능
    dlg->resize(520, 420);

    auto *vlay = new QVBoxLayout(dlg);
    vlay->setSpacing(10);
    vlay->setContentsMargins(16, 16, 16, 12);

    // 이미지 썸네일
    auto *imgLabel = new QLabel(dlg);
    QPixmap px(out);
    if (!px.isNull()) {
        px = px.scaled(480, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imgLabel->setPixmap(px);
        imgLabel->setAlignment(Qt::AlignCenter);
    } else {
        imgLabel->setText("(미리보기 불가)");
        imgLabel->setAlignment(Qt::AlignCenter);
    }
    vlay->addWidget(imgLabel);

    // 파일 경로 라벨
    auto *pathLabel = new QLabel(QString("<small>💾 %1</small>").arg(out), dlg);
    pathLabel->setTextFormat(Qt::RichText);
    pathLabel->setWordWrap(true);
    pathLabel->setAlignment(Qt::AlignCenter);
    vlay->addWidget(pathLabel);

    // 버튼 행
    auto *btnRow = new QHBoxLayout;
    auto *openImgBtn = new QPushButton("🖼  이미지 열기 / Open Image", dlg);
    auto *openFolderBtn = new QPushButton("📂  폴더 열기 / Open Folder", dlg);
    auto *closeBtn = new QPushButton("닫기 / Close", dlg);
    btnRow->addWidget(openImgBtn);
    btnRow->addWidget(openFolderBtn);
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);
    vlay->addLayout(btnRow);

    connect(openImgBtn, &QPushButton::clicked, dlg, [out]{
        QDesktopServices::openUrl(QUrl::fromLocalFile(out));
    });
    connect(openFolderBtn, &QPushButton::clicked, dlg, [targetDir]{
        QDesktopServices::openUrl(QUrl::fromLocalFile(targetDir));
    });
    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::close);

    dlg->show();
    statusBar()->showMessage(tr("저장됨: ") + out, 4000);
}

// ── 기본 앱 등록 ──────────────────────────────────────────────────────────────

void MainWindow::onRegisterFileAssociation()
{
    const QString exePath = QCoreApplication::applicationFilePath();
    const QStringList exts = FileUtils::supportedExtensions();

    bool anyDone = false;

#if defined(Q_OS_WIN)
    // Windows: 레지스트리에 파일 연결 등록
    for (const QString &ext : exts) {
        const QString progId = QString("Trivo.%1").arg(ext.toUpper());
        // HKCU\Software\Classes\.ext
        QSettings extKey(QString("HKEY_CURRENT_USER\\Software\\Classes\\.%1").arg(ext),
                         QSettings::NativeFormat);
        extKey.setValue(".", progId);

        // HKCU\Software\Classes\Trivo.EXT
        QSettings progKey(QString("HKEY_CURRENT_USER\\Software\\Classes\\%1").arg(progId),
                          QSettings::NativeFormat);
        progKey.setValue(".", QString("Trivo 3D File (%1)").arg(ext.toUpper()));
        progKey.setValue("DefaultIcon/.", exePath + ",0");
        progKey.setValue("shell/open/command/.",
            QString("\"%1\" \"%2\"").arg(exePath, "%1"));
        anyDone = true;
    }
    if (anyDone) {
        // Shell 아이콘 캐시 갱신
        QProcess::startDetached("ie4uinit.exe", {"-show"});
        QMessageBox::information(this, "등록 완료 / Registered",
            QString("✅ %1개 3D 형식이 Trivo 기본 앱으로 등록되었습니다.\n\n"
                    "바탕화면 파일 아이콘이 갱신되려면 로그아웃 후 재로그인하거나\n"
                    "탐색기를 재시작하세요.\n\n"
                    "✅ %1 3D formats registered as default for Trivo.\n"
                    "Re-login or restart Explorer to refresh icons.")
            .arg(exts.size()));
    }

#elif defined(Q_OS_LINUX)
    // Linux: xdg-mime + update-desktop-database
    const QString desktopFile = QStandardPaths::writableLocation(
        QStandardPaths::ApplicationsLocation) + "/trivo.desktop";

    // Write desktop file
    QStringList mimeTypes;
    for (const QString &ext : exts)
        mimeTypes << QString("model/x-%1").arg(ext);

    QFile df(desktopFile);
    if (df.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&df);
        ts << "[Desktop Entry]\n"
           << "Name=Trivo\n"
           << "Exec=" << exePath << " %f\n"
           << "Type=Application\n"
           << "MimeType=" << mimeTypes.join(";") << ";\n"
           << "Icon=trivo\n"
           << "Categories=Graphics;3DGraphics;\n";
        df.close();
        anyDone = true;
    }
    for (const QString &ext : exts) {
        QProcess::execute("xdg-mime", {"default", "trivo.desktop",
                          QString("model/x-%1").arg(ext)});
    }
    QProcess::execute("update-desktop-database",
        {QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)});
    if (anyDone)
        QMessageBox::information(this, "등록 완료 / Registered",
            "✅ xdg-mime로 Trivo를 기본 3D 뷰어로 등록했습니다.\n"
            "✅ Registered Trivo as default 3D viewer via xdg-mime.");

#elif defined(Q_OS_MACOS)
    QMessageBox::information(this, "macOS 안내 / macOS Info",
        "macOS에서는 파인더 → 파일 우클릭 → 이 앱으로 열기 → 항상 열기로\n"
        "직접 연결하거나, duti 툴을 사용하세요.\n\n"
        "On macOS: right-click a file in Finder → Open With → Always Open With,\n"
        "or use the 'duti' command-line tool.");
#endif

    (void)anyDone;
}

void MainWindow::onBackgroundColorChanged()
{
    QColor c = QColorDialog::getColor(m_viewport->backgroundColor(), this,
                                      tr("배경색 / Background Color"));
    if (c.isValid()) m_viewport->setBackgroundColor(c);
}

// ── Persist ───────────────────────────────────────────────────────────────────

void MainWindow::saveSettings()
{
    QSettings s;
    s.setValue("geometry",  saveGeometry());
    s.setValue("darkTheme", m_darkTheme);
    s.setValue("language",  m_language);
}

void MainWindow::loadSettings()
{
    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    m_darkTheme = s.value("darkTheme", true).toBool();
    m_language  = s.value("language", "ko").toString();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (m_dropOverlay) m_dropOverlay->setGeometry(rect());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
    e->accept();
}
