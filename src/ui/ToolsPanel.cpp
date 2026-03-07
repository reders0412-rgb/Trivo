#include "ToolsPanel.h"
#include "../core/Scene.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>

ToolsPanel::ToolsPanel(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // ── 도구 목록 ────────────────────────────────────────────────────────────
    auto *toolsGroup = new QGroupBox("🔧 도구 / Tools", this);
    auto *toolsLayout = new QVBoxLayout(toolsGroup);
    toolsLayout->setSpacing(6);

    // Select 도구
    m_selectBtn = new QPushButton("👆 선택 / Select (Q)", this);
    m_selectBtn->setCheckable(true);
    m_selectBtn->setChecked(true);
    m_selectBtn->setStyleSheet(
        "QPushButton { "
        "  background: #3a7bd5; color: white; "
        "  border: 2px solid #5aa0ff; border-radius: 6px; "
        "  padding: 8px; font-weight: bold; "
        "} "
        "QPushButton:hover { background: #4a8be5; } "
        "QPushButton:checked { background: #3a7bd5; border: 2px solid #8fc7ff; }"
    );
    connect(m_selectBtn, &QPushButton::clicked, this, &ToolsPanel::onToolButtonClicked);
    toolsLayout->addWidget(m_selectBtn);

    // Move 도구
    m_moveBtn = new QPushButton("➡️  이동 / Move (G)", this);
    m_moveBtn->setCheckable(true);
    m_moveBtn->setStyleSheet(
        "QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 8px; } "
        "QPushButton:hover { background: #3a3a48; } "
        "QPushButton:checked { background: #3a7bd5; color: white; border: 2px solid #5aa0ff; }"
    );
    connect(m_moveBtn, &QPushButton::clicked, this, &ToolsPanel::onToolButtonClicked);
    toolsLayout->addWidget(m_moveBtn);

    // Rotate 도구
    m_rotateBtn = new QPushButton("🔄 회전 / Rotate (R)", this);
    m_rotateBtn->setCheckable(true);
    m_rotateBtn->setStyleSheet(
        "QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 8px; } "
        "QPushButton:hover { background: #3a3a48; } "
        "QPushButton:checked { background: #3a7bd5; color: white; border: 2px solid #5aa0ff; }"
    );
    connect(m_rotateBtn, &QPushButton::clicked, this, &ToolsPanel::onToolButtonClicked);
    toolsLayout->addWidget(m_rotateBtn);

    // Scale 도구
    m_scaleBtn = new QPushButton("📏 크기 / Scale (S)", this);
    m_scaleBtn->setCheckable(true);
    m_scaleBtn->setStyleSheet(
        "QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a3a48; border-radius: 6px; padding: 8px; } "
        "QPushButton:hover { background: #3a3a48; } "
        "QPushButton:checked { background: #3a7bd5; color: white; border: 2px solid #5aa0ff; }"
    );
    connect(m_scaleBtn, &QPushButton::clicked, this, &ToolsPanel::onToolButtonClicked);
    toolsLayout->addWidget(m_scaleBtn);

    toolsGroup->setStyleSheet("QGroupBox::title { padding: 0px 3px; }");
    mainLayout->addWidget(toolsGroup);

    // ── 도구 정보 ────────────────────────────────────────────────────────────
    auto *infoGroup = new QGroupBox("ℹ️  정보 / Info", this);
    auto *infoLayout = new QVBoxLayout(infoGroup);

    m_infoLabel = new QLabel(
        "🎯 모드: 선택\nMode: Select\n\n"
        "좌클릭: 모델 선택\n"
        "Left Click: Pick Model\n\n"
        "모델을 선택한 후\n다른 도구를 사용하세요.\n\n"
        "Select a model first,\nthen use other tools."
    );
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setStyleSheet("font-size: 11px; color: #aaa; line-height: 1.6;");
    infoLayout->addWidget(m_infoLabel);
    infoLayout->addStretch();

    mainLayout->addWidget(infoGroup);

    // ── 단축키 설명 ────────────────────────────────────────────────────────────
    auto *shortcutGroup = new QGroupBox("⌨️  단축키 / Shortcuts", this);
    auto *shortcutLayout = new QVBoxLayout(shortcutGroup);

    auto *shortcutLabel = new QLabel(
        "<b>변환 도구 / Transform:</b><br>"
        "Q - 선택 / Select<br>"
        "G - 이동 / Move<br>"
        "R - 회전 / Rotate<br>"
        "S - 크기 / Scale<br>"
        "Esc - 취소 / Cancel<br><br>"
        "<b>카메라 / Camera:</b><br>"
        "F - 씬 맞춤 / Fit Scene<br>"
        "R (모델 없음) - 리셋 / Reset<br>"
        "마우스 드래그 - 궤도 / Orbit"
    );
    shortcutLabel->setWordWrap(true);
    shortcutLabel->setStyleSheet("font-size: 10px; color: #888;");
    shortcutLayout->addWidget(shortcutLabel);

    mainLayout->addWidget(shortcutGroup);
    mainLayout->addStretch();

    // 신호 연결
    connect(m_scene.get(), &Scene::selectedModelChanged, this, &ToolsPanel::onModelSelected);
    connect(m_scene.get(), &Scene::sceneCleared, this, [this] {
        m_selectedModelIdx = -1;
        m_infoLabel->setText(
            "🎯 모드: 선택\nMode: Select\n\n"
            "좌클릭: 모델 선택\n"
            "Left Click: Pick Model"
        );
    });
}

void ToolsPanel::setupToolButtons()
{
    // 도구 버튼 초기화 (이미 생성자에서 처리)
}

void ToolsPanel::onToolButtonClicked()
{
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if (!sender) return;

    // 모든 버튼 체크 해제
    m_selectBtn->setChecked(false);
    m_moveBtn->setChecked(false);
    m_rotateBtn->setChecked(false);
    m_scaleBtn->setChecked(false);

    // 클릭된 버튼만 체크
    sender->setChecked(true);

    // 도구 결정
    if (sender == m_selectBtn) {
        updateSelectedTool("select");
        emit toolSelected("select");
    } else if (sender == m_moveBtn) {
        if (m_selectedModelIdx >= 0) {
            updateSelectedTool("move");
            emit toolSelected("move");
        } else {
            sender->setChecked(false);
            m_selectBtn->setChecked(true);
            m_infoLabel->setText(
                "❌ 먼저 모델을 선택해주세요\n"
                "Please select a model first"
            );
        }
    } else if (sender == m_rotateBtn) {
        if (m_selectedModelIdx >= 0) {
            updateSelectedTool("rotate");
            emit toolSelected("rotate");
        } else {
            sender->setChecked(false);
            m_selectBtn->setChecked(true);
            m_infoLabel->setText(
                "❌ 먼저 모델을 선택해주세요\n"
                "Please select a model first"
            );
        }
    } else if (sender == m_scaleBtn) {
        if (m_selectedModelIdx >= 0) {
            updateSelectedTool("scale");
            emit toolSelected("scale");
        } else {
            sender->setChecked(false);
            m_selectBtn->setChecked(true);
            m_infoLabel->setText(
                "❌ 먼저 모델을 선택해주세요\n"
                "Please select a model first"
            );
        }
    }
}

void ToolsPanel::onModelSelected(int index)
{
    m_selectedModelIdx = index;
    
    if (index >= 0) {
        m_infoLabel->setText(
            "✅ 모델 선택됨\n"
            "Model Selected\n\n"
            "다른 도구를 선택하거나\n"
            "마우스로 조작할 수 있습니다.\n\n"
            "Select a tool or\nmanipulate with mouse"
        );
        m_moveBtn->setEnabled(true);
        m_rotateBtn->setEnabled(true);
        m_scaleBtn->setEnabled(true);
    } else {
        m_infoLabel->setText(
            "🎯 모드: 선택\nMode: Select\n\n"
            "좌클릭: 모델 선택\n"
            "Left Click: Pick Model"
        );
        m_moveBtn->setEnabled(true);
        m_rotateBtn->setEnabled(true);
        m_scaleBtn->setEnabled(true);
    }
}

void ToolsPanel::updateSelectedTool(const QString &toolName)
{
    m_activeTool = toolName;
    
    QString infoText;
    if (toolName == "select") {
        infoText = "👆 선택 도구\nSelect Tool\n\n좌클릭으로 모델을 선택합니다.\nLeft click to select model.";
    } else if (toolName == "move") {
        infoText = "➡️  이동 도구\nMove Tool\n\n마우스를 드래그해서\n모델을 이동합니다.\nDrag to move.";
    } else if (toolName == "rotate") {
        infoText = "🔄 회전 도구\nRotate Tool\n\n마우스를 드래그해서\n모델을 회전합니다.\nDrag to rotate.";
    } else if (toolName == "scale") {
        infoText = "📏 크기 도구\nScale Tool\n\n마우스를 드래그해서\n모델 크기를 조정합니다.\nDrag to scale.";
    }
    
    m_infoLabel->setText(infoText);
    emit toolSelected(toolName);
}
