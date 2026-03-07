#pragma once
#include <QWidget>
#include <QColor>
#include <memory>

class QPushButton;
class QLabel;
class QSlider;
class QColorDialog;
class Scene;
class QScrollArea;

class TexturePanel : public QWidget
{
    Q_OBJECT
public:
    explicit TexturePanel(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);

public slots:
    void onModelSelected(int index);
    void onBrushColorChanged();
    void onBrushSizeChanged(int size);
    void onPaintModeToggled();

signals:
    void paintingModeChanged(bool enabled);
    void brushColorChanged(const QColor &color);
    void brushSizeChanged(int size);

private:
    void setupPaintingTools();
    void setupTexturePreview();
    void updateTexturePreview();
    void refreshTextureInfo();

    std::shared_ptr<Scene> m_scene;
    int m_selectedModelIdx = -1;
    
    // 페인팅 도구
    QPushButton *m_colorBtn = nullptr;
    QLabel *m_colorPreview = nullptr;
    QSlider *m_brushSizeSlider = nullptr;
    QLabel *m_brushSizeLabel = nullptr;
    QPushButton *m_paintModeBtn = nullptr;
    bool m_paintingEnabled = false;
    QColor m_brushColor{255, 255, 255, 255};
    int m_brushSize = 10;
    
    // 텍스처 프리뷰
    QLabel *m_texturePreview = nullptr;
    QLabel *m_textureInfoLabel = nullptr;
    QScrollArea *m_textureScroll = nullptr;
};
