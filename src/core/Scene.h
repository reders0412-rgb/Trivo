#pragma once
#include <QObject>
#include <QVector3D>
#include <QString>
#include <QColor>
#include <vector>
#include <memory>

struct SceneModel {
    QString  filePath;
    QString  name;
    QVector3D position{0,0,0};
    QVector3D rotation{0,0,0};
    QVector3D scale{1,1,1};
    bool     visible   = true;
    bool     hasAnim   = false;
    int      vertCount = 0;
    int      triCount  = 0;
    int      meshCount = 0;
    int      matCount  = 0;
    int      boneCount = 0;
    // GPU handle filled by Renderer
    unsigned int vao = 0;
    // raw assimp scene (managed by ModelLoader)
    void *assimpScene = nullptr;
};

class Scene : public QObject
{
    Q_OBJECT

public:
    explicit Scene(QObject *parent = nullptr);

    void addModel(const QString &path);
    void clear();
    void removeModel(int index);

    const std::vector<std::shared_ptr<SceneModel>> &models() const { return m_models; }

    QColor backgroundColor() const { return m_bgColor; }
    void   setBackgroundColor(const QColor &c) { m_bgColor = c; emit backgroundColorChanged(c); }

    // Selection
    int  selectedIndex() const   { return m_selectedIndex; }
    void setSelectedIndex(int i);
    std::shared_ptr<SceneModel> selectedModel() const;

    // Render mode
    bool textureVisible() const  { return m_textureVisible; }
    void setTextureVisible(bool v) { m_textureVisible = v; emit renderModeChanged(); }

    // Global light intensity multiplier
    float lightIntensityMultiplier() const { return m_lightMult; }
    void  setLightIntensityMultiplier(float v) { m_lightMult = v; emit lightMultiplierChanged(v); }

signals:
    void modelAdded(std::shared_ptr<SceneModel>);
    void modelRemoved(int index);
    void sceneCleared();
    void backgroundColorChanged(const QColor &c);
    void selectionChanged(int index);
    void renderModeChanged();
    void lightMultiplierChanged(float v);

private:
    std::vector<std::shared_ptr<SceneModel>> m_models;
    QColor m_bgColor{30, 30, 38};
    int    m_selectedIndex = -1;
    bool   m_textureVisible = true;
    float  m_lightMult = 1.0f;
};
