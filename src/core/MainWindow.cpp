#include "MainWindow.h"
#include "Viewport.h"
#include "Scene.h"
#include "../ui/Sidebar.h"
#include "../ui/ToolBar.h"
#include "../ui/InfoPanel.h"
#include "../ui/AboutDialog.h"
#include "../ui/DropOverlay.h"
#include "../i18n/I18n.h"
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
    retranslateUI();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

// ── Setup ─────────────────────────────────────────────────────────────────────

void MainWindow::setupUI()
{
    // Central widget
    auto *central = new QWidget(this);
    setCentralWidget(central);

    // Toolbar
    m_toolbar = new ToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_toolbar);

    // Viewport
    m_viewport = new Viewport(m_scene, central);
    m_viewport->setMinimumWidth(600);

    // Right sidebar
    m_sidebar = new Sidebar(m_scene, central);
    m_sidebar->setFixedWidth(300);

    // Splitter
    auto *splitter = new QSplitter(Qt::Horizontal, central);
    splitter->addWidget(m_viewport);
    splitter->addWidget(m_sidebar);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setHandleWidth(2);

    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(splitter);

    // Drop overlay (shown when dragging files)
    m_dropOverlay = new DropOverlay(this);
    m_dropOverlay->hide();

    // Info panel (status bar area)
    m_infoPanel = new InfoPanel(this);
    statusBar()->addPermanentWidget(m_infoPanel, 1);

    // Connections
    connect(m_toolbar, &ToolBar::openFileRequested,    this, &MainWindow::onOpenFile);
    connect(m_toolbar, &ToolBar::addFileRequested,     this, &MainWindow::onAddFile);
    connect(m_toolbar, &ToolBar::clearSceneRequested,  this, &MainWindow::onClearScene);
    connect(m_toolbar, &ToolBar::themeToggled,         this, &MainWindow::onToggleTheme);
    connect(m_toolbar, &ToolBar::languageChanged,      this, &MainWindow::onLanguageChanged);
    connect(m_toolbar, &ToolBar::resetCameraRequested, this, &MainWindow::onResetCamera);
    connect(m_toolbar, &ToolBar::screenshotRequested,  this, &MainWindow::onScreenshot);
    connect(m_toolbar, &ToolBar::animationToggled,     this, &MainWindow::onToggleAnimation);
    connect(m_toolbar, &ToolBar::animationSpeedChanged,this, &MainWindow::onAnimationSpeedChanged);

    connect(m_sidebar, &Sidebar::backgroundColorChangeRequested,
            this, &MainWindow::onBackgroundColorChanged);
    connect(m_sidebar, &Sidebar::lightPresetChanged,
            this, &MainWindow::onLightPresetChanged);
    connect(m_sidebar, &Sidebar::aboutRequested, this, &MainWindow::onAbout);

    connect(m_scene.get(), &Scene::modelAdded, this, [this](std::shared_ptr<SceneModel>){ m_sidebar->refresh(); });
    connect(m_scene.get(), &Scene::modelAdded, m_infoPanel, &InfoPanel::refresh);
    connect(m_scene.get(), &Scene::sceneCleared, m_infoPanel, &InfoPanel::clear);
    connect(m_scene.get(), &Scene::sceneCleared, m_sidebar, &Sidebar::refresh);
}

void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu(tr("파일 / File"));
    fileMenu->addAction(QIcon(":/icons/open.ico"),   tr("열기…"), this, &MainWindow::onOpenFile, QKeySequence::Open);
    fileMenu->addAction(QIcon(":/icons/add.ico"),    tr("씬에 추가…"), this, &MainWindow::onAddFile, QKeySequence("Ctrl+Shift+O"));
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/icons/camera.ico"), tr("스크린샷 저장"), this, &MainWindow::onScreenshot, QKeySequence("Ctrl+P"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("종료"), this, &QMainWindow::close, QKeySequence::Quit);

    auto *viewMenu = menuBar()->addMenu(tr("보기 / View"));
    viewMenu->addAction(tr("카메라 초기화"), this, &MainWindow::onResetCamera, QKeySequence("R"));
    viewMenu->addAction(tr("씬 비우기"), this, &MainWindow::onClearScene);
    viewMenu->addSeparator();
    viewMenu->addAction(tr("다크 / 라이트 전환"), this, &MainWindow::onToggleTheme, QKeySequence("Ctrl+T"));

    auto *helpMenu = menuBar()->addMenu(tr("도움말 / Help"));
    helpMenu->addAction(tr("Trivo 정보…"), this, &MainWindow::onAbout);
}

// ── Theme ─────────────────────────────────────────────────────────────────────

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
        QToolButton { background: transparent; color: #e8e8ec; border-radius: 6px; padding: 6px 10px; }
        QToolButton:hover { background: #2d2d38; }
        QToolButton:pressed { background: #3a7bd5; }
        QSplitter::handle { background: #2a2a32; }
        QStatusBar { background: #141418; color: #888; border-top: 1px solid #2a2a32; }
        QScrollBar:vertical { background: #1a1a1f; width: 8px; }
        QScrollBar::handle:vertical { background: #3a3a48; border-radius: 4px; }
        QScrollBar::handle:vertical:hover { background: #4a4a58; }
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
        QGroupBox { color: #888; border: 1px solid #2a2a32; border-radius: 8px; margin-top: 8px; padding-top: 8px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #666; font-size: 10px; }
        QSpinBox, QDoubleSpinBox { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 3px 6px; }
    )" : R"(
        QMainWindow, QWidget { background: #f5f5f8; color: #1a1a2e; }
        QMenuBar { background: #ffffff; color: #1a1a2e; border-bottom: 1px solid #ddd; }
        QMenuBar::item:selected { background: #e8f0fe; }
        QMenu { background: #ffffff; color: #1a1a2e; border: 1px solid #ddd; }
        QMenu::item:selected { background: #3a7bd5; color: white; }
        QToolBar { background: #ffffff; border-bottom: 1px solid #ddd; spacing: 4px; padding: 4px; }
        QToolButton { background: transparent; color: #1a1a2e; border-radius: 6px; padding: 6px 10px; }
        QToolButton:hover { background: #e8f0fe; }
        QToolButton:pressed { background: #3a7bd5; color: white; }
        QSplitter::handle { background: #ddd; }
        QStatusBar { background: #ffffff; color: #666; border-top: 1px solid #ddd; }
        QScrollBar:vertical { background: #f5f5f8; width: 8px; }
        QScrollBar::handle:vertical { background: #ccc; border-radius: 4px; }
        QLabel { color: #1a1a2e; }
        QPushButton { background: #e8f0fe; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 6px 14px; }
        QPushButton:hover { background: #3a7bd5; color: white; border-color: #3a7bd5; }
        QComboBox { background: #ffffff; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 4px 8px; }
        QTreeWidget { background: #ffffff; color: #1a1a2e; border: none; }
        QTreeWidget::item:hover { background: #e8f0fe; }
        QTreeWidget::item:selected { background: #3a7bd5; color: white; }
        QGroupBox { color: #666; border: 1px solid #ddd; border-radius: 8px; margin-top: 8px; padding-top: 8px; }
        QSlider::groove:horizontal { background: #ddd; height: 4px; border-radius: 2px; }
        QSlider::handle:horizontal { background: #3a7bd5; width: 14px; height: 14px; border-radius: 7px; margin: -5px 0; }
        QSpinBox, QDoubleSpinBox { background: #ffffff; color: #1a1a2e; border: 1px solid #ccc; border-radius: 6px; padding: 3px 6px; }
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
        this,
        tr("3D 파일 열기 / Open 3D File"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        FileUtils::filterString()
    );
    if (!path.isEmpty()) openFile(path);
}

void MainWindow::onAddFile()
{
    const QStringList paths = QFileDialog::getOpenFileNames(
        this,
        tr("씬에 모델 추가 / Add Models to Scene"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        FileUtils::filterString()
    );
    for (const QString &p : paths) m_scene->addModel(p);
}

void MainWindow::onClearScene()
{
    m_scene->clear();
    setWindowTitle("Trivo");
}

// ── Drag & Drop ───────────────────────────────────────────────────────────────

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        for (const QUrl &u : e->mimeData()->urls()) {
            if (FileUtils::isSupported(u.toLocalFile())) {
                e->acceptProposedAction();
                if (m_dropOverlay) m_dropOverlay->show();
                return;
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if (m_dropOverlay) m_dropOverlay->hide();
    bool first = true;
    for (const QUrl &u : e->mimeData()->urls()) {
        const QString path = u.toLocalFile();
        if (FileUtils::isSupported(path)) {
            if (first) {
                m_scene->clear();
                first = false;
            }
            m_scene->addModel(path);
        }
    }
    if (!first) m_viewport->resetCamera();
}

// ── Actions ───────────────────────────────────────────────────────────────────

void MainWindow::onToggleTheme()      { applyTheme(!m_darkTheme); }
void MainWindow::onResetCamera()      { m_viewport->resetCamera(); }
void MainWindow::onToggleAnimation()  { m_viewport->toggleAnimation(); }
void MainWindow::onAnimationSpeedChanged(double s) { m_viewport->setAnimationSpeed(s); }
void MainWindow::onLightPresetChanged(int i) { m_viewport->setLightPreset(i); }

void MainWindow::onLanguageChanged(const QString &lang)
{
    I18n::instance().setLanguage(lang);
    retranslateUI();
}

void MainWindow::onAbout()
{
    AboutDialog dlg(m_darkTheme, this);
    dlg.exec();
}

void MainWindow::onScreenshot()
{
    // 1. 진짜 '문서' 폴더 경로 가져오기
    const QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    
    // 2. 문서 폴더 안에 'trivo' 폴더 경로 설정
    const QString targetDir = docPath + "/trivo";

    // 3. 'trivo' 폴더가 없으면 새로 만들기 (중요!)
    QDir dir;
    if (!dir.exists(targetDir)) {
        dir.mkpath(targetDir); 
    }

    // 4. 파일 이름 결정
    const QString ts  = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString out = targetDir + "/trivo_" + ts + ".png";

    // 5. 캡처 및 저장
    QPixmap px = m_viewport->grab();
    if (px.save(out)) {
        statusBar()->showMessage(tr("저장됨: ") + out, 4000);
    }
}

void MainWindow::onBackgroundColorChanged()
{
    QColor c = QColorDialog::getColor(m_viewport->backgroundColor(), this, tr("배경색 / Background Color"));
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

void MainWindow::retranslateUI()
{
    // menus & toolbar will pick up the language from I18n
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (m_dropOverlay)
        m_dropOverlay->setGeometry(rect());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
    e->accept();
}
