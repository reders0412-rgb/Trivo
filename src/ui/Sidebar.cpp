#include "Sidebar.h"
#include "ScenePanel.h"
#include "../core/Scene.h"
#include "../core/LightSystem.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>

Sidebar::Sidebar(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabs = new QTabWidget(this);

    // ── Scene tab ─────────────────────────────────────────────────────────────
    m_scenePanel = new ScenePanel(scene, this);
    m_tabs->addTab(m_scenePanel, "씬 / Scene");

    // ── Environment tab ───────────────────────────────────────────────────────
    auto *envWidget = new QWidget(this);
    auto *envLayout = new QVBoxLayout(envWidget);
    envLayout->setContentsMargins(12, 12, 12, 12);
    envLayout->setSpacing(10);

    // ── Background ────────────────────────────────────────────────────────────
    auto *bgGroup = new QGroupBox("배경 / Background", envWidget);
    auto *bgLayout = new QVBoxLayout(bgGroup);
    auto *bgBtn = new QPushButton("🎨  배경색 선택 / Pick Color", bgGroup);
    connect(bgBtn, &QPushButton::clicked,
            this, &Sidebar::backgroundColorChangeRequested);
    bgLayout->addWidget(bgBtn);
    envLayout->addWidget(bgGroup);

    // ── Light preset ──────────────────────────────────────────────────────────
    auto *lightGroup = new QGroupBox("라이트 프리셋 / Light Preset", envWidget);
    auto *lightLayout = new QVBoxLayout(lightGroup);
    auto *lightCombo = new QComboBox(lightGroup);
    for (const auto &p : LightSystem::presets())
        lightCombo->addItem(p.name);
    connect(lightCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Sidebar::lightPresetChanged);
    lightLayout->addWidget(lightCombo);
    envLayout->addWidget(lightGroup);

    // ── Light Intensity ───────────────────────────────────────────────────────
    auto *intGroup = new QGroupBox("조명 밝기 / Light Intensity", envWidget);
    auto *intLayout = new QVBoxLayout(intGroup);

    // 슬라이더 범위: 25~800 (실제값: 0.25×~8.0×), 기본 400 = 4.0×
    // 값/100.0 = 배수
    auto *intRow = new QHBoxLayout;
    m_lightIntLabel = new QLabel("4.0×", intGroup);
    m_lightIntLabel->setFixedWidth(38);
    m_lightIntLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_lightIntSlider = new QSlider(Qt::Horizontal, intGroup);
    m_lightIntSlider->setRange(25, 800);   // 0.25× ~ 8.0×
    m_lightIntSlider->setValue(400);       // 기본 4.0×
    m_lightIntSlider->setTickInterval(100);
    m_lightIntSlider->setToolTip("0.25× ~ 8.0×  (기본 4.0×)");

    intRow->addWidget(new QLabel("0.25×", intGroup));
    intRow->addWidget(m_lightIntSlider, 1);
    intRow->addWidget(new QLabel("8×", intGroup));
    intLayout->addLayout(intRow);
    intLayout->addWidget(m_lightIntLabel, 0, Qt::AlignHCenter);

    connect(m_lightIntSlider, &QSlider::valueChanged,
            this, &Sidebar::onLightIntensitySliderChanged);
    envLayout->addWidget(intGroup);

    // 초기값으로 씬에 적용
    m_scene->setLightIntensityMultiplier(4.0f);

    // ── Ambient (그림자 밝기) ─────────────────────────────────────────────────
    auto *ambGroup = new QGroupBox("그림자 밝기 / Ambient", envWidget);
    auto *ambLayout = new QVBoxLayout(ambGroup);

    auto *ambRow = new QHBoxLayout;
    m_ambientLabel = new QLabel("15%", ambGroup);
    m_ambientLabel->setFixedWidth(38);
    m_ambientLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_ambientSlider = new QSlider(Qt::Horizontal, ambGroup);
    m_ambientSlider->setRange(0, 100);  // 0% ~ 100%
    m_ambientSlider->setValue(15);      // 기본 0.15
    m_ambientSlider->setToolTip("주변광 강도 — 높을수록 그림자가 밝아짐\nAmbient strength — higher = brighter shadows");

    ambRow->addWidget(new QLabel("0%", ambGroup));
    ambRow->addWidget(m_ambientSlider, 1);
    ambRow->addWidget(new QLabel("100%", ambGroup));
    ambLayout->addLayout(ambRow);
    ambLayout->addWidget(m_ambientLabel, 0, Qt::AlignHCenter);

    connect(m_ambientSlider, &QSlider::valueChanged,
            this, &Sidebar::onAmbientSliderChanged);
    envLayout->addWidget(ambGroup);

    // ── Shadow toggle (소프트 그림자 힌트) ───────────────────────────────────
    auto *shadowGroup = new QGroupBox("렌더 옵션 / Render Options", envWidget);
    auto *shadowLayout = new QVBoxLayout(shadowGroup);
    m_shadowCheck = new QCheckBox("소프트 그림자 / Soft Shadows", shadowGroup);
    m_shadowCheck->setChecked(false);
    m_shadowCheck->setToolTip("주변광 강도를 올려 그림자를 부드럽게 합니다\nIncreases ambient to soften shadows");
    connect(m_shadowCheck, &QCheckBox::toggled, this, &Sidebar::onShadowToggled);
    shadowLayout->addWidget(m_shadowCheck);
    envLayout->addWidget(shadowGroup);

    envLayout->addStretch();

    // ── About ─────────────────────────────────────────────────────────────────
    auto *aboutBtn = new QPushButton("ℹ  Trivo 정보 / About", envWidget);
    connect(aboutBtn, &QPushButton::clicked, this, &Sidebar::aboutRequested);
    envLayout->addWidget(aboutBtn);

    m_tabs->addTab(envWidget, "환경 / Env");
    layout->addWidget(m_tabs);
}

void Sidebar::refresh()
{
    if (m_scenePanel) m_scenePanel->refresh();
}

void Sidebar::syncSelection(int index)
{
    if (m_scenePanel) m_scenePanel->syncSelection(index);
}

// ── Slots ──────────────────────────────────────────────────────────────────────

void Sidebar::onLightIntensitySliderChanged(int value)
{
    double mult = value / 100.0;
    updateLightIntensityLabel(mult);
    m_scene->setLightIntensityMultiplier(static_cast<float>(mult));
}

void Sidebar::onAmbientSliderChanged(int value)
{
    updateAmbientLabel(value);
    // Pass to scene; Viewport will forward to LightSystem
    float strength = value / 100.0f;
    m_scene->setAmbientStrength(strength);
}

void Sidebar::onShadowToggled(bool on)
{
    // Soft shadows: boost ambient to ~40%, or restore to slider value
    if (on) {
        m_ambientSlider->setValue(40);
    } else {
        m_ambientSlider->setValue(15);
    }
}

void Sidebar::updateLightIntensityLabel(double mult)
{
    m_lightIntLabel->setText(QString("%1×").arg(mult, 0, 'f', 2));
}

void Sidebar::updateAmbientLabel(int value)
{
    m_ambientLabel->setText(QString("%1%").arg(value));
}
