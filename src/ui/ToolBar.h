#pragma once
#include <QToolBar>

class QAction;
class QDoubleSpinBox;

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QWidget *parent = nullptr);

    void updateThemeIcon(bool dark);
    // 기즈모 버튼을 외부에서 체크 상태 변경
    void setGizmoMode(int mode);

signals:
    void openFileRequested();
    void addFileRequested();
    void clearSceneRequested();
    void themeToggled();
    void resetCameraRequested();
    void screenshotRequested();
    void animationToggled();
    void animationSpeedChanged(double speed);
    // Gizmo: 0=none 1=translate 2=rotate 3=scale
    void gizmoModeChanged(int mode);
    // Texture visible toggle
    void textureVisibleChanged(bool visible);

private:
    QAction        *m_themeAction    = nullptr;
    QAction        *m_playAction     = nullptr;
    QDoubleSpinBox *m_speedSpin      = nullptr;
    QAction        *m_gizmoNone      = nullptr;
    QAction        *m_gizmoTranslate = nullptr;
    QAction        *m_gizmoRotate    = nullptr;
    QAction        *m_gizmoScale     = nullptr;
    QAction        *m_textureAction  = nullptr;
    bool            m_textureVisible = true;
};
