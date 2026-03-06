#pragma once
#include <QWidget>
#include <QColor>
#include <memory>

class QPushButton;
class QLabel;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class Scene;

class SkyboxPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SkyboxPanel(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);

public slots:
    void onSkyboxTypeChanged(int index);
    void onSkyboxColorChanged();
    void onIntensityChanged(double value);
    void onSkyboxRotationChanged(double value);

signals:
    void skyboxTypeChanged(int type); // 0: 색상, 1: 그라디언트, 2: 텍스처
    void skyboxColorChanged(const QColor &color);
    void skyboxIntensityChanged(double intensity);
    void skyboxRotationChanged(double rotation);
    void skyboxLoadTextureRequested(const QString &path);

private:
    void setupSkyboxUI();
    void updateSkyboxPreview();

private slots:
    void onRotationChanged(int value);

    std::shared_ptr<Scene> m_scene;
    
    // 스카이박스 타입
    QComboBox *m_skyboxTypeCombo = nullptr;
    
    // 색상 옵션
    QPushButton *m_colorBtn = nullptr;
    QLabel *m_colorPreview = nullptr;
    QColor m_skyboxColor{100, 150, 200, 255};
    
    // 그라디언트 옵션
    QPushButton *m_topColorBtn = nullptr;
    QPushButton *m_bottomColorBtn = nullptr;
    QLabel *m_gradientPreview = nullptr;
    QColor m_topColor{135, 206, 235, 255}; // 하늘색
    QColor m_bottomColor{255, 228, 181, 255}; // 모래색
    
    // 공통 옵션
    QDoubleSpinBox *m_intensitySpin = nullptr;
    QDoubleSpinBox *m_rotationSpin = nullptr;
    
    // 텍스처 옵션
    QPushButton *m_loadTextureBtn = nullptr;
    QLabel *m_texturePathLabel = nullptr;
};
