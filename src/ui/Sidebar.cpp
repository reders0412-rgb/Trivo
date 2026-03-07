#include "Sidebar.h"
#include "ScenePanel.h"
#include "../core/Scene.h"
#include "../core/LightSystem.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

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

    // Background
    auto *bgGroup = new QGroupBox("배경 / Background", envWidget);
    auto *bgLayout = new QVBoxLayout(bgGroup);
    auto *bgBtn = new QPushButton("배경색 선택 / Pick Color", bgGroup);
    connect(bgBtn, &QPushButton::clicked,
            this, &Sidebar::backgroundColorChangeRequested);
    bgLayout->addWidget(bgBtn);
    envLayout->addWidget(bgGroup);

    // Light preset
    auto *lightGroup = new QGroupBox("라이트 / Lighting", envWidget);
    auto *lightLayout = new QVBoxLayout(lightGroup);
    auto *lightCombo = new QComboBox(lightGroup);
    for (const auto &p : LightSystem::presets())
        lightCombo->addItem(p.name);
    connect(lightCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Sidebar::lightPresetChanged);
    lightLayout->addWidget(lightCombo);
    envLayout->addWidget(lightGroup);

    envLayout->addStretch();

    // About button
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
