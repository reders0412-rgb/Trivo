#include "Viewport.h"
#include "Scene.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>
#include <cmath>
#include <limits>

Viewport::Viewport(std::shared_ptr<Scene> scene, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_scene(scene)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_frameTimer, &QTimer::timeout, this, &Viewport::onFrameTick);
    m_frameTimer.start(16);

    connect(scene.get(), &Scene::modelAdded, this, &Viewport::onModelAdded);
    connect(scene.get(), &Scene::sceneCleared, this, [this]{ update(); });
    connect(scene.get(), &Scene::selectionChanged, this, [this](int){ update(); });
    connect(scene.get(), &Scene::renderModeChanged, this, [this]{ update(); });
    connect(scene.get(), &Scene::lightMultiplierChanged, this, [this](float){ update(); });
}

Viewport::~Viewport() = default;

// ── OpenGL ─────────────────────────────────────────────────────────────────────

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

// ── Frame tick ─────────────────────────────────────────────────────────────────

void Viewport::onFrameTick()
{
    m_anim.tick();
    update();
}

void Viewport::onModelAdded()
{
    const auto &models = m_scene->models();
    if (!models.empty()) {
        QVector3D center{0,0,0};
        for (const auto &m : models) center += m->position;
        center /= static_cast<float>(models.size());
        m_camera.fitToScene(center, 3.f);
    }
    update();
}

// ── Public API ─────────────────────────────────────────────────────────────────

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

void Viewport::setGizmoMode(GizmoMode mode) { m_gizmoMode = mode; }

// ── Screenshot (씬 뷰만) ────────────────────────────────────────────────────────

void Viewport::takeScreenshot(const QString &outputPath)
{
    // grabFramebuffer() captures only this OpenGL widget — no UI chrome
    QImage img = grabFramebuffer();
    img.save(outputPath, "PNG");
}

// ── Picking (ray-AABB per model) ───────────────────────────────────────────────

void Viewport::screenToRay(const QPoint &p, QVector3D &rayOrigin, QVector3D &rayDir) const
{
    // NDC
    float ndcX = (2.f * p.x() / width())  - 1.f;
    float ndcY = 1.f - (2.f * p.y() / height());

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;
    QMatrix4x4 proj = m_camera.projectionMatrix(aspect);
    QMatrix4x4 view = m_camera.viewMatrix();

    QMatrix4x4 invVP = (proj * view).inverted();

    QVector4D near4 = invVP * QVector4D(ndcX, ndcY, -1.f, 1.f);
    QVector4D far4  = invVP * QVector4D(ndcX, ndcY,  1.f, 1.f);

    QVector3D nearPt = QVector3D(near4) / near4.w();
    QVector3D farPt  = QVector3D(far4)  / far4.w();

    rayOrigin = nearPt;
    rayDir    = (farPt - nearPt).normalized();
}

// Simple ray-sphere intersection (using bounding radius estimate)
static bool raySphereIntersect(const QVector3D &ro, const QVector3D &rd,
                                const QVector3D &center, float radius)
{
    QVector3D oc = ro - center;
    float b = QVector3D::dotProduct(oc, rd);
    float c = QVector3D::dotProduct(oc, oc) - radius * radius;
    float disc = b*b - c;
    return disc >= 0.f && (-b - std::sqrt(disc)) > 0.f;
}

int Viewport::pickModel(const QPoint &screenPos)
{
    QVector3D ro, rd;
    screenToRay(screenPos, ro, rd);

    const auto &models = m_scene->models();
    float bestT = std::numeric_limits<float>::max();
    int   bestI = -1;

    for (int i = 0; i < (int)models.size(); ++i) {
        const auto &mdl = *models[i];
        if (!mdl.visible) continue;

        // Bounding sphere in world space: estimate radius from scale
        float radius = std::max({mdl.scale.x(), mdl.scale.y(), mdl.scale.z()}) * 1.5f;
        radius = std::max(radius, 0.5f);

        if (raySphereIntersect(ro, rd, mdl.position, radius)) {
            // Approximate distance
            float t = QVector3D::dotProduct(mdl.position - ro, rd);
            if (t < bestT) { bestT = t; bestI = i; }
        }
    }
    return bestI;
}

// ── Gizmo drag ─────────────────────────────────────────────────────────────────

void Viewport::applyGizmoDrag(const QPoint &delta)
{
    auto mdl = m_scene->selectedModel();
    if (!mdl) return;

    // Compute world-space right/up from camera
    QVector3D forward = (m_camera.target() - m_camera.position()).normalized();
    QVector3D right   = QVector3D::crossProduct(forward, {0,1,0}).normalized();
    QVector3D up      = QVector3D::crossProduct(right, forward).normalized();

    float sensitivity = m_camera.target().distanceToPoint(QVector3D()) * 0.002f + 0.005f;

    float dx = delta.x() * sensitivity;
    float dy = -delta.y() * sensitivity;

    if (m_gizmoMode == GizmoMode::Translate) {
        mdl->position += right * dx + up * dy;
    } else if (m_gizmoMode == GizmoMode::Rotate) {
        mdl->rotation += QVector3D(dy * 40.f, dx * 40.f, 0.f);
    } else if (m_gizmoMode == GizmoMode::Scale) {
        float s = 1.f + (dx + dy);
        mdl->scale *= std::max(0.01f, s);
    }
}

// ── Input ──────────────────────────────────────────────────────────────────────

void Viewport::mousePressEvent(QMouseEvent *e)
{
    m_lastMousePos = e->pos();
    m_activeButton = e->button();
    m_draggingGizmo = false;
    setFocus();

    if (e->button() == Qt::LeftButton) {
        // Try gizmo drag on selected model first
        int sel = m_scene->selectedIndex();
        if (sel >= 0 && m_gizmoMode != GizmoMode::None) {
            // Check if click is near the selected model's projected center
            const auto &mdl = *m_scene->models()[sel];
            float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;
            QMatrix4x4 mvp = m_camera.projectionMatrix(aspect) * m_camera.viewMatrix();
            QVector4D clip = mvp * QVector4D(mdl.position, 1.f);
            if (clip.w() > 0.f) {
                QVector3D ndc = QVector3D(clip) / clip.w();
                QPoint projPt(
                    static_cast<int>((ndc.x() + 1.f) * 0.5f * width()),
                    static_cast<int>((1.f - ndc.y()) * 0.5f * height())
                );
                int dist = (e->pos() - projPt).manhattanLength();
                if (dist < 80) {
                    m_draggingGizmo = true;
                    return;
                }
            }
        }

        // Click picking
        int hit = pickModel(e->pos());
        m_scene->setSelectedIndex(hit);
        emit modelSelected(hit);
    }
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint delta = e->pos() - m_lastMousePos;
    m_lastMousePos = e->pos();

    if (m_activeButton == Qt::LeftButton && m_draggingGizmo) {
        applyGizmoDrag(delta);
        return;
    }

    if (m_activeButton == Qt::LeftButton && !m_draggingGizmo) {
        m_camera.orbit(delta.x() * 0.4f, delta.y() * 0.4f);
    } else if (m_activeButton == Qt::MiddleButton ||
               (e->buttons() & Qt::RightButton && e->modifiers() & Qt::ShiftModifier)) {
        m_camera.pan(delta.x(), delta.y());
    } else if (m_activeButton == Qt::RightButton) {
        m_camera.orbit(delta.x() * 0.4f, delta.y() * 0.4f);
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent *)
{
    m_activeButton  = Qt::NoButton;
    m_draggingGizmo = false;
}

void Viewport::wheelEvent(QWheelEvent *e)
{
    float d = e->angleDelta().y() / 120.f;
    m_camera.zoom(d);
}

void Viewport::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_R:
        if (e->modifiers() == Qt::NoModifier) { resetCamera(); }
        break;
    case Qt::Key_F:  onModelAdded(); break;
    case Qt::Key_Space: toggleAnimation(); break;
    // Gizmo shortcuts (W/E/R like Unity — but R conflicts with reset)
    case Qt::Key_W:
        m_gizmoMode = GizmoMode::Translate;
        emit modelSelected(m_scene->selectedIndex()); // refresh sidebar
        break;
    case Qt::Key_E:
        m_gizmoMode = GizmoMode::Rotate;
        break;
    case Qt::Key_Q:
        m_gizmoMode = GizmoMode::None;
        break;
    default: QOpenGLWidget::keyPressEvent(e);
    }
}
