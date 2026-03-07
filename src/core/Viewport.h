#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <memory>
#include "Camera.h"
#include "LightSystem.h"
#include "AnimationPlayer.h"

class Scene;
class Renderer;

enum class GizmoMode { None = 0, Translate = 1, Rotate = 2, Scale = 3 };

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

    // 씬 뷰만 캡처 (깜빡임 없음)
    void takeScreenshot(const QString &outputPath);

    void setGizmoMode(GizmoMode mode);
    GizmoMode gizmoMode() const { return m_gizmoMode; }

signals:
    void modelSelected(int index);
    void gizmoModeChangedByKey(int mode);  // 키보드로 변경 시 툴바 동기화

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
    // --- Picking ---
    int  pickModel(const QPoint &pos);
    void screenToRay(const QPoint &p, QVector3D &ro, QVector3D &rd) const;

    // --- Gizmo GL 오버레이 ---
    void setupGizmoShader();
    void drawGizmoOverlay();
    // 기즈모 축 화살표 드로우 (NDC 공간)
    void drawAxisArrow(const QVector3D &origin, const QVector3D &dir,
                       const QVector4D &color, float len);

    // --- Drag ---
    void applyGizmoDrag(const QPoint &delta);

    std::shared_ptr<Scene>    m_scene;
    std::unique_ptr<Renderer> m_renderer;
    Camera                    m_camera;
    LightSystem               m_lights;
    AnimationPlayer           m_anim;
    QTimer                    m_frameTimer;
    QPoint                    m_lastMousePos;
    Qt::MouseButton           m_activeButton   = Qt::NoButton;
    bool                      m_glReady        = false;
    bool                      m_draggingGizmo  = false;

    GizmoMode   m_gizmoMode = GizmoMode::None;

    // Gizmo overlay shader
    std::unique_ptr<QOpenGLShaderProgram> m_gizmoShader;
    unsigned int m_gizmoVao = 0, m_gizmoVbo = 0;
};
