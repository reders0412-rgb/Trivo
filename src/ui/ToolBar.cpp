#include "ToolBar.h"
#include "../utils/IconProvider.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QAction>
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
    addAction(IconProvider::resetIcon(), "카메라 초기화 / Reset Camera  [R]",
              this, &ToolBar::resetCameraRequested);

    addSeparator();

    // ── Gizmo mode (Q/W/E — R은 카메라 리셋에 사용) ──────────────────────────
    auto *gizmoGroup = new QActionGroup(this);
    gizmoGroup->setExclusive(true);

    m_gizmoNone = new QAction("✖", this);
    m_gizmoNone->setToolTip("선택 없음 / No Gizmo  [Q]");
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
    m_gizmoScale->setToolTip("크기 / Scale  [T]");
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
    m_textureAction->setToolTip("PBR 렌더 ↔ 와이어프레임 / PBR ↔ Wireframe");
    m_textureAction->setCheckable(true);
    m_textureAction->setChecked(true);
    addAction(m_textureAction);
    connect(m_textureAction, &QAction::toggled, this, [this](bool on){
        m_textureVisible = on;
        m_textureAction->setText(on ? "🖼" : "⬜");
        emit textureVisibleChanged(on);
    });

    addSeparator();

    // ── Animation ─────────────────────────────────────────────────────────────
    m_playAction = addAction(IconProvider::playIcon(), "재생/정지 / Play/Pause  [Space]",
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

    // ── Screenshot ───────────────────────────────────────────────────────────
    addAction(IconProvider::screenshotIcon(), "스크린샷 / Screenshot  [Ctrl+P]",
              this, &ToolBar::screenshotRequested);

    // ── Spacer + Theme ────────────────────────────────────────────────────────
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    m_themeAction = addAction(IconProvider::darkThemeIcon(), "다크/라이트 / Theme  [Ctrl+T]",
                              this, &ToolBar::themeToggled);
}

void ToolBar::updateThemeIcon(bool dark)
{
    if (m_themeAction)
        m_themeAction->setIcon(dark ? IconProvider::lightThemeIcon()
                                    : IconProvider::darkThemeIcon());
}

void ToolBar::setGizmoMode(int mode)
{
    // 단축키로 모드 변경 시 툴바 버튼도 동기화
    switch (mode) {
    case 0: if (m_gizmoNone)      m_gizmoNone->setChecked(true);      break;
    case 1: if (m_gizmoTranslate) m_gizmoTranslate->setChecked(true); break;
    case 2: if (m_gizmoRotate)    m_gizmoRotate->setChecked(true);     break;
    case 3: if (m_gizmoScale)     m_gizmoScale->setChecked(true);      break;
    }
}
