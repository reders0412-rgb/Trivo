#include "ToolBar.h"
#include "../utils/IconProvider.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QAction>

ToolBar::ToolBar(QWidget *parent) : QToolBar(parent)
{
    setMovable(false);
    setIconSize({22,22});

    // Open
    addAction(IconProvider::openIcon(), "열기 / Open",
              this, &ToolBar::openFileRequested);

    // Add to scene
    addAction(IconProvider::addIcon(), "씬에 추가 / Add to Scene",
              this, &ToolBar::addFileRequested);

    addSeparator();

    // Reset camera
    addAction(IconProvider::resetIcon(), "카메라 초기화 / Reset Camera",
              this, &ToolBar::resetCameraRequested);

    // Screenshot
    addAction(IconProvider::screenshotIcon(), "스크린샷 / Screenshot",
              this, &ToolBar::screenshotRequested);

    addSeparator();

    // Play/pause animation
    m_playAction = addAction(IconProvider::playIcon(), "재생/정지 / Play/Pause",
                             this, &ToolBar::animationToggled);

    // Animation speed
    auto *speedLabel = new QLabel("  속도 / Speed: ", this);
    addWidget(speedLabel);
    m_speedSpin = new QDoubleSpinBox(this);
    m_speedSpin->setRange(0.1, 10.0);
    m_speedSpin->setSingleStep(0.1);
    m_speedSpin->setValue(1.0);
    m_speedSpin->setDecimals(1);
    m_speedSpin->setFixedWidth(70);
    addWidget(m_speedSpin);
    connect(m_speedSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ToolBar::animationSpeedChanged);

    addSeparator();

    // Spacer
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    addSeparator();

    // Theme toggle
    m_themeAction = addAction(IconProvider::darkThemeIcon(), "다크/라이트 / Theme",
                              this, &ToolBar::themeToggled);
}

void ToolBar::updateThemeIcon(bool dark)
{
    if (m_themeAction)
        m_themeAction->setIcon(dark ? IconProvider::lightThemeIcon()
                                    : IconProvider::darkThemeIcon());
}
