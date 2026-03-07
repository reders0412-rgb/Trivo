#include "TexturePanel.h"
#include "../core/Scene.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QScrollArea>
#include <QPixmap>
#include <QFileInfo>
#include <QPainter>

TexturePanel::TexturePanel(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // ── 페인팅 도구 ────────────────────────────────────────────────────────────
    setupPaintingTools();
    auto *paintGroup = new QGroupBox("🎨 페인팅 / Painting", this);
    auto *paintLayout = new QVBoxLayout(paintGroup);

    // 색상 선택
    auto *colorRow = new QHBoxLayout;
    colorRow->addWidget(new QLabel("색상 / Color:"));
    m_colorPreview = new QLabel(this);
    m_colorPreview->setFixedSize(40, 40);
    m_colorPreview->setStyleSheet("border: 1px solid #888; border-radius: 4px;");
    updateTexturePreview(); // 초기 색상 표시
    colorRow->addWidget(m_colorPreview);
    m_colorBtn = new QPushButton("선택 / Pick", this);
    m_colorBtn->setMaximumWidth(100);
    connect(m_colorBtn, &QPushButton::clicked, this, &TexturePanel::onBrushColorChanged);
    colorRow->addWidget(m_colorBtn);
    colorRow->addStretch();
    paintLayout->addLayout(colorRow);

    // 브러시 크기
    auto *brushRow = new QHBoxLayout;
    brushRow->addWidget(new QLabel("크기 / Size:"));
    m_brushSizeSlider = new QSlider(Qt::Horizontal, this);
    m_brushSizeSlider->setRange(1, 50);
    m_brushSizeSlider->setValue(10);
    m_brushSizeSlider->setTickPosition(QSlider::TicksBelow);
    m_brushSizeSlider->setTickInterval(10);
    connect(m_brushSizeSlider, &QSlider::valueChanged, this, &TexturePanel::onBrushSizeChanged);
    brushRow->addWidget(m_brushSizeSlider);
    m_brushSizeLabel = new QLabel("10 px", this);
    m_brushSizeLabel->setMinimumWidth(50);
    brushRow->addWidget(m_brushSizeLabel);
    paintLayout->addLayout(brushRow);

    // 페인팅 모드 토글
    m_paintModeBtn = new QPushButton("🖌  페인팅 시작 / Start Painting", this);
    m_paintModeBtn->setStyleSheet(
        "QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a7bd5; }"
        "QPushButton:checked { background: #3a7bd5; color: white; }"
    );
    m_paintModeBtn->setCheckable(true);
    connect(m_paintModeBtn, &QPushButton::clicked, this, &TexturePanel::onPaintModeToggled);
    paintLayout->addWidget(m_paintModeBtn);

    // 사용 방법
    auto *tipLabel = new QLabel(
        "💡 팁: 모델을 선택한 후 페인팅 시작을 누르세요.\n"
        "마우스로 모델 표면을 드래그해서 칠할 수 있습니다.\n"
        "Tip: Select a model, then click Start Painting.\n"
        "Drag on the model surface to paint."
    );
    tipLabel->setWordWrap(true);
    tipLabel->setStyleSheet("font-size: 10px; color: #888;");
    paintLayout->addWidget(tipLabel);

    paintLayout->addStretch();
    mainLayout->addWidget(paintGroup);

    // ── 텍스처 프리뷰 ────────────────────────────────────────────────────────────
    setupTexturePreview();
    auto *texGroup = new QGroupBox("🖼  텍스처 / Texture Preview", this);
    auto *texLayout = new QVBoxLayout(texGroup);

    m_textureInfoLabel = new QLabel("모델을 선택하세요 / Select a model", this);
    m_textureInfoLabel->setWordWrap(true);
    m_textureInfoLabel->setStyleSheet("font-size: 11px; color: #888;");
    texLayout->addWidget(m_textureInfoLabel);

    // 텍스처 프리뷰 스크롤
    m_textureScroll = new QScrollArea(this);
    m_textureScroll->setWidgetResizable(true);
    m_textureScroll->setMinimumHeight(200);
    m_texturePreview = new QLabel(this);
    m_texturePreview->setAlignment(Qt::AlignCenter);
    m_texturePreview->setStyleSheet("background: #1a1a1f; border: 1px solid #3a3a48;");
    m_textureScroll->setWidget(m_texturePreview);
    texLayout->addWidget(m_textureScroll);

    texLayout->addStretch();
    mainLayout->addWidget(texGroup);

    mainLayout->addStretch();

    // 신호 연결
    connect(m_scene.get(), &Scene::selectedModelChanged, this, &TexturePanel::onModelSelected);
    connect(m_scene.get(), &Scene::sceneCleared, this, [this] {
        m_selectedModelIdx = -1;
        m_texturePreview->clear();
        m_textureInfoLabel->setText("모델을 선택하세요 / Select a model");
    });
}

void TexturePanel::setupPaintingTools()
{
    // 페인팅 도구 초기화 (미래 구현용)
}

void TexturePanel::setupTexturePreview()
{
    // 텍스처 프리뷰 초기화 (미래 구현용)
}

void TexturePanel::onModelSelected(int index)
{
    m_selectedModelIdx = index;
    refreshTextureInfo();
    updateTexturePreview();

    // 선택된 모델에 따라 페인팅 모드 비활성화
    if (m_paintingEnabled) {
        m_paintModeBtn->setChecked(false);
        onPaintModeToggled();
    }
}

void TexturePanel::onBrushColorChanged()
{
    QColor color = QColorDialog::getColor(m_brushColor, this, "브러시 색상 선택 / Select Brush Color");
    if (color.isValid()) {
        m_brushColor = color;
        
        // 색상 프리뷰 업데이트
        QPixmap colorPixmap(40, 40);
        colorPixmap.fill(m_brushColor);
        m_colorPreview->setPixmap(colorPixmap);
        
        emit brushColorChanged(m_brushColor);
    }
}

void TexturePanel::onBrushSizeChanged(int size)
{
    m_brushSize = size;
    m_brushSizeLabel->setText(QString("%1 px").arg(size));
    emit brushSizeChanged(size);
}

void TexturePanel::onPaintModeToggled()
{
    m_paintingEnabled = m_paintModeBtn->isChecked();
    
    if (m_selectedModelIdx < 0) {
        m_paintModeBtn->setChecked(false);
        m_paintingEnabled = false;
        m_textureInfoLabel->setText("❌ 먼저 모델을 선택해주세요 / Please select a model first");
        return;
    }

    if (m_paintingEnabled) {
        m_paintModeBtn->setText("🎨 페인팅 중... / Painting Active");
        m_paintModeBtn->setStyleSheet(
            "QPushButton { background: #3a7bd5; color: white; border: 2px solid #5aa0ff; }"
        );
        m_textureInfoLabel->setText("✨ 페인팅 모드 활성화됨 / Painting mode is active");
    } else {
        m_paintModeBtn->setText("🖌  페인팅 시작 / Start Painting");
        m_paintModeBtn->setStyleSheet(
            "QPushButton { background: #2d2d38; color: #e8e8ec; border: 1px solid #3a7bd5; }"
        );
        m_textureInfoLabel->setText("완료됨 / Done");
    }

    emit paintingModeChanged(m_paintingEnabled);
}

void TexturePanel::updateTexturePreview()
{
    if (m_selectedModelIdx < 0) {
        QPixmap placeholder(200, 200);
        placeholder.fill(QColor(50, 50, 60));
        m_texturePreview->setPixmap(placeholder);
        return;
    }

    const auto &models = m_scene->models();
    if (m_selectedModelIdx >= static_cast<int>(models.size())) {
        return;
    }

    auto model = models[m_selectedModelIdx];
    
    // 현재 브러시 색상을 프리뷰로 표시
    QPixmap preview(200, 200);
    preview.fill(QColor(30, 30, 38));
    
    // 중앙에 큰 색상 블록 표시
    QPixmap colorBlock(100, 100);
    colorBlock.fill(m_brushColor);
    
    QPainter painter(&preview);
    painter.drawPixmap(50, 50, colorBlock);
    painter.drawRect(49, 49, 101, 101); // 테두리
    painter.end();
    
    m_texturePreview->setPixmap(preview);
}

void TexturePanel::refreshTextureInfo()
{
    if (m_selectedModelIdx < 0) {
        m_textureInfoLabel->setText("모델을 선택하세요 / Select a model");
        return;
    }

    const auto &models = m_scene->models();
    if (m_selectedModelIdx >= static_cast<int>(models.size())) {
        return;
    }

    auto model = models[m_selectedModelIdx];
    
    QString info = QString(
        "📦 모델: %1\n"
        "🔹 정점 색상: %2개\n"
        "🎨 텍스처: 미지원 (향후 추가 예정)\n"
        "\n"
        "💾 저장: 페인팅이 자동으로 정점 색상에 저장됩니다\n"
        "Export: Vertex colors are automatically saved"
    ).arg(QFileInfo(model->filePath).baseName())
     .arg(model->vertCount);
    
    m_textureInfoLabel->setText(info);
}
