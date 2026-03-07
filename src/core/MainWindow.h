#pragma once
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <memory>
#include "../core/Viewport.h"

class Sidebar;
class ToolBar;
class InfoPanel;
class Scene;
class DropOverlay;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFile(const QString &path);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void onOpenFile();
    void onAddFile();
    void onClearScene();
    void onToggleTheme();
    void onAbout();
    void onResetCamera();
    void onScreenshot();
    void onToggleAnimation();
    void onAnimationSpeedChanged(double speed);
    void onBackgroundColorChanged();
    void onLightPresetChanged(int index);
    void onModelSelected(int index);
    void onGizmoModeChanged(int mode);
    void onTextureVisibleChanged(bool visible);
    void onRegisterFileAssociation();

private:
    void setupUI();
    void setupMenuBar();
    void applyTheme(bool dark);
    void saveSettings();
    void loadSettings();

    Viewport    *m_viewport    = nullptr;
    Sidebar     *m_sidebar     = nullptr;
    ToolBar     *m_toolbar     = nullptr;
    InfoPanel   *m_infoPanel   = nullptr;
    DropOverlay *m_dropOverlay = nullptr;

    std::shared_ptr<Scene> m_scene;

    bool    m_darkTheme = true;
    QString m_language  = "ko";
};
