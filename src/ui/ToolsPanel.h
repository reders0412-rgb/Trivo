#pragma once
#include <QWidget>
#include <memory>

class QPushButton;
class QLabel;
class Scene;

class ToolsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ToolsPanel(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);

public slots:
    void onToolButtonClicked();
    void onModelSelected(int index);

signals:
    void toolSelected(const QString &toolName);

private:
    void setupToolButtons();
    void updateSelectedTool(const QString &toolName);

    std::shared_ptr<Scene> m_scene;
    int m_selectedModelIdx = -1;
    
    QPushButton *m_selectBtn = nullptr;
    QPushButton *m_moveBtn = nullptr;
    QPushButton *m_rotateBtn = nullptr;
    QPushButton *m_scaleBtn = nullptr;
    
    QLabel *m_infoLabel = nullptr;
    QString m_activeTool = "select";
};
