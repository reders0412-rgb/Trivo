#pragma once
#include <QWidget>
#include <memory>

class Scene;
class ScenePanel;
class QTabWidget;
class QSlider;
class QLabel;
class QCheckBox;

class Sidebar : public QWidget
{
    Q_OBJECT
public:
    explicit Sidebar(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);

    void refresh();
    void syncSelection(int index);

signals:
    void backgroundColorChangeRequested();
    void lightPresetChanged(int index);
    void aboutRequested();

private slots:
    void onLightIntensitySliderChanged(int value);
    void onAmbientSliderChanged(int value);
    void onShadowToggled(bool on);

private:
    void updateLightIntensityLabel(double mult);
    void updateAmbientLabel(int value);

    std::shared_ptr<Scene> m_scene;
    ScenePanel *m_scenePanel       = nullptr;
    QTabWidget *m_tabs             = nullptr;

    // Env tab controls
    QSlider    *m_lightIntSlider   = nullptr;
    QLabel     *m_lightIntLabel    = nullptr;
    QSlider    *m_ambientSlider    = nullptr;
    QLabel     *m_ambientLabel     = nullptr;
    QCheckBox  *m_shadowCheck      = nullptr;
};
