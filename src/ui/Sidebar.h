#pragma once
#include <QWidget>
#include <memory>

class Scene;
class ScenePanel;
class QTabWidget;

class Sidebar : public QWidget
{
    Q_OBJECT
public:
    explicit Sidebar(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);

public slots:
    void refresh();

signals:
    void backgroundColorChangeRequested();
    void lightPresetChanged(int index);
    void aboutRequested();

private:
    std::shared_ptr<Scene> m_scene;
    ScenePanel *m_scenePanel = nullptr;
    QTabWidget *m_tabs       = nullptr;
};
