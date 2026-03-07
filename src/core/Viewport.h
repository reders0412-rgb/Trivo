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

// Gizmo mode: 0=none, 1=translate, 2=rotate, 3=scale
enum class GizmoMode { None = 0, Translate = 1, Rotate = 2, Scale = 3 };
// Gizmo axis: 0=none, 1=X, 2=Y, 3=Z
enum class GizmoAxis { None = 0, X = 1, Y = 2, Z = 3 };

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

    // Screenshot of the scene viewport only
    void takeScreenshot(const QString &outputPath);

    // Gizmo mode
    void setGizmoMode(GizmoMode mode);

signals:
    void modelSelected(int index);

protected:
    void initializeGL()                    override;
    void resizeGL(int w, int h)            override;
    void paintGL()                         override;
    void mousePressEvent(QMouseEvent *e)   override;
    void mouseMoveEvent(QMouseEvent *e)    override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e)        override;
    void keyPressEvent(QKeyEvent *e)       override;

private slots:
    void onFrameTick();
    void onModelAdded();

private:
    // Ray-cast picking: returns model index or -1
    int  pickModel(const QPoint &screenPos);
    // Convert screen delta → world-space gizmo delta
    void applyGizmoDrag(const QPoint &delta);
    // Compute a ray in world space from screen position
    void screenToRay(const QPoint &p, QVector3D &rayOrigin, QVector3D &rayDir) const;

    std::shared_ptr<Scene>    m_scene;
    std::unique_ptr<Renderer> m_renderer;
    Camera                    m_camera;
    LightSystem               m_lights;
    AnimationPlayer           m_anim;
    QTimer                    m_frameTimer;
    QPoint                    m_lastMousePos;
    Qt::MouseButton           m_activeButton  = Qt::NoButton;
    bool                      m_glReady       = false;

    GizmoMode   m_gizmoMode    = GizmoMode::None;
    bool        m_draggingGizmo = false;  // true when dragging selected model
};
