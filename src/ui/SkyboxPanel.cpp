#include "SkyboxPanel.h"
#include "../core/Scene.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>

SkyboxPanel::SkyboxPanel(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // ── 스카이박스 설정 ────────────────────────────────────────────────────────
    setupSkyboxUI();

    auto *skyboxGroup = new QGroupBox("🌅 스카이박스 / Skybox", this);
    auto *skyboxLayout = new QVBoxLayout(skyboxGroup);

    // 타입 선택
    auto *typeRow = new QHBoxLayout;
    typeRow->addWidget(new QLabel("타입 / Type:"));
    m_skyboxTypeCombo = new QComboBox(this);
    m_skyboxTypeCombo->addItem("💜 단색 / Solid Color");
    m_skyboxTypeCombo->addItem("🌈 그라디언트 / Gradient");
    m_skyboxTypeCombo->addItem("🖼  텍스처 / Texture (향후)");
    m_skyboxTypeCombo->setCurrentIndex(0);
    connect(m_skyboxTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SkyboxPanel::onSkyboxTypeChanged);
    typeRow->addWidget(m_skyboxTypeCombo);
    typeRow->addStretch();
    skyboxLayout->addLayout(typeRow);

    // ── 단색 옵션 ──────────────────────────────────────────────────────────────
    auto *solidGroup = new QGroupBox("단색 설정 / Solid Options", this);
    auto *solidLayout = new QVBoxLayout(solidGroup);

    auto *colorRow = new QHBoxLayout;
    colorRow->addWidget(new QLabel("색상 / Color:"));
    m_colorPreview = new QLabel(this);
    m_colorPreview->setFixedSize(50, 50);
    m_colorPreview->setStyleSheet("border: 1px solid #666; border-radius: 4px;");
    QPixmap colorPixmap(50, 50);
    colorPixmap.fill(m_skyboxColor);
    m_colorPreview->setPixmap(colorPixmap);
    colorRow->addWidget(m_colorPreview);
    m_colorBtn = new QPushButton("선택 / Pick", this);
    m_colorBtn->setMaximumWidth(100);
    connect(m_colorBtn, &QPushButton::clicked, this, &SkyboxPanel::onSkyboxColorChanged);
    colorRow->addWidget(m_colorBtn);
    colorRow->addStretch();
    solidLayout->addLayout(colorRow);

    solidGroup->setVisible(true);
    skyboxLayout->addWidget(solidGroup);

    // ── 그라디언트 옵션 ────────────────────────────────────────────────────────
    auto *gradientGroup = new QGroupBox("그라디언트 설정 / Gradient Options", this);
    auto *gradientLayout = new QVBoxLayout(gradientGroup);

    // 상단 색상
    auto *topColorRow = new QHBoxLayout;
    topColorRow->addWidget(new QLabel("상단 / Top:"));
    m_topColorBtn = new QPushButton("상단 색상 / Top Color", this);
    m_topColorBtn->setMaximumWidth(150);
    topColorRow->addWidget(m_topColorBtn);
    topColorRow->addStretch();
    gradientLayout->addLayout(topColorRow);

    // 하단 색상
    auto *bottomColorRow = new QHBoxLayout;
    bottomColorRow->addWidget(new QLabel("하단 / Bottom:"));
    m_bottomColorBtn = new QPushButton("하단 색상 / Bottom Color", this);
    m_bottomColorBtn->setMaximumWidth(150);
    bottomColorRow->addWidget(m_bottomColorBtn);
    bottomColorRow->addStretch();
    gradientLayout->addLayout(bottomColorRow);

    // 그라디언트 프리뷰
    m_gradientPreview = new QLabel(this);
    m_gradientPreview->setFixedHeight(60);
    m_gradientPreview->setStyleSheet("border: 1px solid #666; border-radius: 4px;");
    QPixmap gradPixmap(200, 60);
    QPainter painter(&gradPixmap);
    QLinearGradient gradient(0, 0, 0, 60);
    gradient.setColorAt(0, m_topColor);
    gradient.setColorAt(1, m_bottomColor);
    painter.fillRect(gradPixmap.rect(), gradient);
    painter.end();
    m_gradientPreview->setPixmap(gradPixmap);
    gradientLayout->addWidget(m_gradientPreview);

    gradientGroup->setVisible(false);
    skyboxLayout->addWidget(gradientGroup);

    // ── 공통 설정 ────────────────────────────────────────────────────────────
    auto *commonGroup = new QGroupBox("공통 설정 / Common Settings", this);
    auto *commonLayout = new QVBoxLayout(commonGroup);

    // 밝기
    auto *intensityRow = new QHBoxLayout;
    intensityRow->addWidget(new QLabel("밝기 / Intensity:"));
    m_intensitySpin = new QDoubleSpinBox(this);
    m_intensitySpin->setRange(0.0, 3.0);
    m_intensitySpin->setValue(1.0);
    m_intensitySpin->setSingleStep(0.1);
    m_intensitySpin->setDecimals(1);
    connect(m_intensitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SkyboxPanel::onIntensityChanged);
    intensityRow->addWidget(m_intensitySpin);
    intensityRow->addStretch();
    commonLayout->addLayout(intensityRow);

    // 회전
    auto *rotationRow = new QHBoxLayout;
    rotationRow->addWidget(new QLabel("회전 / Rotation:"));
    m_rotationSpin = new QDoubleSpinBox(this);
    m_rotationSpin->setRange(0.0, 360.0);
    m_rotationSpin->setValue(0.0);
    m_rotationSpin->setSingleStep(15.0);
    m_rotationSpin->setDecimals(0);
    m_rotationSpin->setSuffix(" °");
    connect(m_rotationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SkyboxPanel::onSkyboxRotationChanged);
    rotationRow->addWidget(m_rotationSpin);
    rotationRow->addStretch();
    commonLayout->addLayout(rotationRow);

    skyboxLayout->addWidget(commonGroup);

    // ── 텍스처 옵션 ────────────────────────────────────────────────────────────
    auto *textureGroup = new QGroupBox("텍스처 설정 / Texture Options (미리보기)", this);
    auto *textureLayout = new QVBoxLayout(textureGroup);

    m_loadTextureBtn = new QPushButton("📁 텍스처 로드 / Load Texture", this);
    m_loadTextureBtn->setEnabled(false); // 향후 활성화
    textureLayout->addWidget(m_loadTextureBtn);

    m_texturePathLabel = new QLabel("준비 중... / Coming Soon", this);
    m_texturePathLabel->setWordWrap(true);
    m_texturePathLabel->setStyleSheet("font-size: 10px; color: #888;");
    textureLayout->addWidget(m_texturePathLabel);

    textureGroup->setVisible(false);
    skyboxLayout->addWidget(textureGroup);

    skyboxLayout->addStretch();
    mainLayout->addWidget(skyboxGroup);

    // ── 팁 ────────────────────────────────────────────────────────────────────
    auto *tipLabel = new QLabel(
        "💡 팁:\n"
        "• 단색: 빠르고 간단한 배경\n"
        "• 그라디언트: 하늘-지면 효과\n"
        "• 텍스처: 고급 환경 맵핑 (향후)\n\n"
        "Tip: Choose background type for your scene"
    );
    tipLabel->setWordWrap(true);
    tipLabel->setStyleSheet("font-size: 10px; color: #666;");
    mainLayout->addWidget(tipLabel);

    mainLayout->addStretch();
}

void SkyboxPanel::setupSkyboxUI()
{
    // 향후 구현용
}

void SkyboxPanel::onSkyboxTypeChanged(int index)
{
    // 타입에 따라 UI 업데이트
    auto *solidGroup = findChild<QGroupBox*>();
    auto *gradientGroup = findChild<QGroupBox*>(nullptr, Qt::FindChildrenRecursively);
    
    emit skyboxTypeChanged(index);
    updateSkyboxPreview();
}

void SkyboxPanel::onSkyboxColorChanged()
{
    QColor color = QColorDialog::getColor(m_skyboxColor, this, "스카이박스 색상 선택 / Select Skybox Color");
    if (color.isValid()) {
        m_skyboxColor = color;
        
        QPixmap colorPixmap(50, 50);
        colorPixmap.fill(m_skyboxColor);
        m_colorPreview->setPixmap(colorPixmap);
        
        emit skyboxColorChanged(m_skyboxColor);
        updateSkyboxPreview();
    }
}

void SkyboxPanel::onIntensityChanged(double value)
{
    emit skyboxIntensityChanged(value);
    updateSkyboxPreview();
}

void SkyboxPanel::onSkyboxRotationChanged(double value)
{
    emit skyboxRotationChanged(value);
    updateSkyboxPreview();
}

void SkyboxPanel::updateSkyboxPreview()
{
    // 프리뷰 업데이트 (시각적 피드백)
}
