#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <memory>
#include "Camera.h"
#include "LightSystem.h"
#include "AnimationPlayer.h"

class Scene;
class Renderer;

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    explicit Viewport(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);
    ~Viewport() override;

    void resetCamera();
    void toggleAnimation();
    void setAnimationSpeed(double s);
    void setLightPreset(int index);
    void setBackgroundColor(const QColor &c);
    QColor backgroundColor() const;

protected:
    void initializeGL()                   override;
    void resizeGL(int w, int h)           override;
    void paintGL()                        override;
    void mousePressEvent(QMouseEvent *e)  override;
    void mouseMoveEvent(QMouseEvent *e)   override;
    void mouseReleaseEvent(QMouseEvent *e)override;
    void wheelEvent(QWheelEvent *e)       override;
    void keyPressEvent(QKeyEvent *e)      override;

private slots:
    void onFrameTick();
    void onModelAdded();

private:
    std::shared_ptr<Scene>    m_scene;
    std::unique_ptr<Renderer> m_renderer;
    Camera                    m_camera;
    LightSystem               m_lights;
    AnimationPlayer           m_anim;
    QTimer                    m_frameTimer;
    QPoint                    m_lastMousePos;
    Qt::MouseButton           m_activeButton = Qt::NoButton;
    bool                      m_glReady      = false;
};
