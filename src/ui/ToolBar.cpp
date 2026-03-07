#include "ToolBar.h"
#include "../utils/IconProvider.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QAction>
#include <QSlider>
#include <QWidget>
#include <QActionGroup>

ToolBar::ToolBar(QWidget *parent) : QToolBar(parent)
{
    setMovable(false);
    setIconSize({22,22});

    // ── File ────────────────────────────────────────────────────────────────
    addAction(IconProvider::openIcon(), "열기 / Open",
              this, &ToolBar::openFileRequested);
    addAction(IconProvider::addIcon(), "씬에 추가 / Add to Scene",
              this, &ToolBar::addFileRequested);

    addSeparator();

    // ── Camera ───────────────────────────────────────────────────────────────
    addAction(IconProvider::resetIcon(), "카메라 초기화 / Reset Camera",
              this, &ToolBar::resetCameraRequested);

    addSeparator();

    // ── Gizmo mode (W/E/R like Unity) ────────────────────────────────────────
    auto *gizmoGroup = new QActionGroup(this);
    gizmoGroup->setExclusive(true);

    m_gizmoNone = new QAction("✖", this);
    m_gizmoNone->setToolTip("선택 해제 / No Gizmo");
    m_gizmoNone->setCheckable(true);
    m_gizmoNone->setChecked(true);
    gizmoGroup->addAction(m_gizmoNone);
    addAction(m_gizmoNone);

    m_gizmoTranslate = new QAction("↔", this);
    m_gizmoTranslate->setToolTip("이동 / Translate  [W]");
    m_gizmoTranslate->setCheckable(true);
    gizmoGroup->addAction(m_gizmoTranslate);
    addAction(m_gizmoTranslate);

    m_gizmoRotate = new QAction("↻", this);
    m_gizmoRotate->setToolTip("회전 / Rotate  [E]");
    m_gizmoRotate->setCheckable(true);
    gizmoGroup->addAction(m_gizmoRotate);
    addAction(m_gizmoRotate);

    m_gizmoScale = new QAction("⤢", this);
    m_gizmoScale->setToolTip("크기 / Scale  [R]");
    m_gizmoScale->setCheckable(true);
    gizmoGroup->addAction(m_gizmoScale);
    addAction(m_gizmoScale);

    connect(m_gizmoNone,      &QAction::triggered, this, [this]{ emit gizmoModeChanged(0); });
    connect(m_gizmoTranslate, &QAction::triggered, this, [this]{ emit gizmoModeChanged(1); });
    connect(m_gizmoRotate,    &QAction::triggered, this, [this]{ emit gizmoModeChanged(2); });
    connect(m_gizmoScale,     &QAction::triggered, this, [this]{ emit gizmoModeChanged(3); });

    addSeparator();

    // ── Texture toggle ────────────────────────────────────────────────────────
    m_textureAction = new QAction("🖼", this);
    m_textureAction->setToolTip("텍스처 표시 / Texture Visible");
    m_textureAction->setCheckable(true);
    m_textureAction->setChecked(true);
    addAction(m_textureAction);
    connect(m_textureAction, &QAction::toggled, this, [this](bool on){
        m_textureVisible = on;
        m_textureAction->setText(on ? "🖼" : "⬜");
        m_textureAction->setToolTip(on ? "텍스처 표시 / Texture Visible"
                                       : "와이어프레임 / Wireframe");
        emit textureVisibleChanged(on);
    });

    addSeparator();

    // ── Animation ─────────────────────────────────────────────────────────────
    m_playAction = addAction(IconProvider::playIcon(), "재생/정지 / Play/Pause",
                             this, &ToolBar::animationToggled);

    auto *speedLabel = new QLabel("  속도:", this);
    addWidget(speedLabel);
    m_speedSpin = new QDoubleSpinBox(this);
    m_speedSpin->setRange(0.1, 10.0);
    m_speedSpin->setSingleStep(0.1);
    m_speedSpin->setValue(1.0);
    m_speedSpin->setDecimals(1);
    m_speedSpin->setFixedWidth(65);
    m_speedSpin->setToolTip("애니메이션 속도 / Animation Speed");
    addWidget(m_speedSpin);
    connect(m_speedSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ToolBar::animationSpeedChanged);

    addSeparator();

    // ── Light intensity ───────────────────────────────────────────────────────
    auto *lightLabel = new QLabel("  💡:", this);
    lightLabel->setToolTip("조명 밝기 / Light Intensity");
    addWidget(lightLabel);

    m_lightSlider = new QSlider(Qt::Horizontal, this);
    m_lightSlider->setRange(0, 400);   // 0.00 ~ 4.00
    m_lightSlider->setValue(100);      // default 1.0×
    m_lightSlider->setFixedWidth(90);
    m_lightSlider->setToolTip("조명 밝기 / Light Intensity  (0× ~ 4×)");
    addWidget(m_lightSlider);
    connect(m_lightSlider, &QSlider::valueChanged, this, [this](int v){
        emit lightIntensityChanged(v / 100.0);
    });

    addSeparator();

    // ── Screenshot ───────────────────────────────────────────────────────────
    addAction(IconProvider::screenshotIcon(), "스크린샷 / Screenshot",
              this, &ToolBar::screenshotRequested);

    // ── Spacer + Theme ────────────────────────────────────────────────────────
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    m_themeAction = addAction(IconProvider::darkThemeIcon(), "다크/라이트 / Theme",
                              this, &ToolBar::themeToggled);
}

void ToolBar::updateThemeIcon(bool dark)
{
    if (m_themeAction)
        m_themeAction->setIcon(dark ? IconProvider::lightThemeIcon()
                                    : IconProvider::darkThemeIcon());
}
