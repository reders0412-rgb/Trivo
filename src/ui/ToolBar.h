#pragma once
#include <QToolBar>

class QAction;
class QDoubleSpinBox;
class QSlider;
class QLabel;

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QWidget *parent = nullptr);

    void updateThemeIcon(bool dark);

signals:
    void openFileRequested();
    void addFileRequested();
    void clearSceneRequested();
    void themeToggled();
    void resetCameraRequested();
    void screenshotRequested();
    void animationToggled();
    void animationSpeedChanged(double speed);
    // Gizmo mode: 0=none 1=translate 2=rotate 3=scale
    void gizmoModeChanged(int mode);
    // Texture visible toggle
    void textureVisibleChanged(bool visible);
    // Light intensity multiplier (0.0 ~ 4.0)
    void lightIntensityChanged(double multiplier);

private:
    QAction        *m_themeAction    = nullptr;
    QAction        *m_playAction     = nullptr;
    QDoubleSpinBox *m_speedSpin      = nullptr;
    // Gizmo actions (mutually exclusive)
    QAction        *m_gizmoNone      = nullptr;
    QAction        *m_gizmoTranslate = nullptr;
    QAction        *m_gizmoRotate    = nullptr;
    QAction        *m_gizmoScale     = nullptr;
    // Texture toggle
    QAction        *m_textureAction  = nullptr;
    bool            m_textureVisible = true;
    // Light intensity
    QSlider        *m_lightSlider    = nullptr;
};
