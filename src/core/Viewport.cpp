#include "Viewport.h"
#include "Scene.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <cmath>

Viewport::Viewport(std::shared_ptr<Scene> scene, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_scene(scene)
{
    setFocusPolicy(Qt::StrongFocus);

    // 60 fps target
    connect(&m_frameTimer, &QTimer::timeout, this, &Viewport::onFrameTick);
    m_frameTimer.start(16);

    connect(scene.get(), &Scene::modelAdded, this, &Viewport::onModelAdded);
    connect(scene.get(), &Scene::sceneCleared, this, [this]{ update(); });
}

Viewport::~Viewport() = default;

// ── OpenGL ────────────────────────────────────────────────────────────────────

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();
    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();
    m_glReady = true;
}

void Viewport::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Viewport::paintGL()
{
    if (!m_glReady || !m_renderer) return;

    const QColor bg = m_scene->backgroundColor();
    glClearColor(bg.redF(), bg.greenF(), bg.blueF(), 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;

    m_renderer->render(
        m_scene,
        m_camera.viewMatrix(),
        m_camera.projectionMatrix(aspect),
        m_camera.position(),
        m_lights,
        m_anim.currentTime()
    );
}

// ── Frame tick ────────────────────────────────────────────────────────────────

void Viewport::onFrameTick()
{
    m_anim.tick();
    update();
}

void Viewport::onModelAdded()
{
    // Fit camera to new scene bounds
    const auto &models = m_scene->models();
    if (!models.empty()) {
        // Simple bounding estimate: use center of all model positions
        QVector3D center{0,0,0};
        for (const auto &m : models) center += m->position;
        center /= static_cast<float>(models.size());
        m_camera.fitToScene(center, 3.f);
    }
    update();
}

// ── Controls ──────────────────────────────────────────────────────────────────

void Viewport::resetCamera()
{
    m_camera.reset();
    onModelAdded();
}

void Viewport::toggleAnimation()
{
    if (m_anim.isPlaying()) m_anim.pause();
    else                    m_anim.play();
}

void Viewport::setAnimationSpeed(double s) { m_anim.setSpeed(s); }

void Viewport::setLightPreset(int index)
{
    m_lights.applyPreset(index);
    update();
}

void Viewport::setBackgroundColor(const QColor &c)
{
    m_scene->setBackgroundColor(c);
    update();
}

QColor Viewport::backgroundColor() const { return m_scene->backgroundColor(); }

// ── Mouse ─────────────────────────────────────────────────────────────────────

void Viewport::mousePressEvent(QMouseEvent *e)
{
    m_lastMousePos = e->pos();
    m_activeButton = e->button();
    setFocus();
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint delta = e->pos() - m_lastMousePos;
    m_lastMousePos = e->pos();

    if (m_activeButton == Qt::LeftButton) {
        m_camera.orbit(delta.x() * 0.4f, delta.y() * 0.4f);
    } else if (m_activeButton == Qt::MiddleButton ||
               (e->buttons() & Qt::RightButton && e->modifiers() & Qt::ShiftModifier)) {
        m_camera.pan(delta.x(), delta.y());
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent *) { m_activeButton = Qt::NoButton; }

void Viewport::wheelEvent(QWheelEvent *e)
{
    float d = e->angleDelta().y() / 120.f;
    m_camera.zoom(d);
}

void Viewport::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_R:  resetCamera();                         break;
    case Qt::Key_F:  onModelAdded();                        break;
    case Qt::Key_Space: toggleAnimation();                  break;
    default: QOpenGLWidget::keyPressEvent(e);
    }
}
