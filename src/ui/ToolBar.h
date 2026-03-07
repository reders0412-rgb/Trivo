#pragma once
#include <QToolBar>

class QAction;
class QComboBox;
class QDoubleSpinBox;

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
    void languageChanged(const QString &lang);
    void resetCameraRequested();
    void screenshotRequested();
    void animationToggled();
    void animationSpeedChanged(double speed);

private:
    QAction     *m_themeAction = nullptr;
    QAction     *m_playAction  = nullptr;
    QComboBox   *m_langCombo   = nullptr;
    QDoubleSpinBox *m_speedSpin = nullptr;
};
