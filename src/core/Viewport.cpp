#include "Viewport.h"
#include "Scene.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <cmath>
#include <limits>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Gizmo overlay shaders (simple colored lines in clip space)
// ─────────────────────────────────────────────────────────────────────────────
static const char *GIZMO_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP;
void main(){ gl_Position = uMVP * vec4(aPos, 1.0); }
)GLSL";

static const char *GIZMO_FRAG = R"GLSL(
#version 410 core
out vec4 fragColor;
uniform vec4 uColor;
void main(){ fragColor = uColor; }
)GLSL";

// ─────────────────────────────────────────────────────────────────────────────

Viewport::Viewport(std::shared_ptr<Scene> scene, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_scene(scene)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_frameTimer, &QTimer::timeout, this, &Viewport::onFrameTick);
    m_frameTimer.start(16);

    connect(scene.get(), &Scene::modelAdded,    this, &Viewport::onModelAdded);
    connect(scene.get(), &Scene::sceneCleared,  this, [this]{ update(); });
    connect(scene.get(), &Scene::selectionChanged,    this, [this](int){ update(); });
    connect(scene.get(), &Scene::renderModeChanged,   this, [this]{ update(); });
    connect(scene.get(), &Scene::lightMultiplierChanged, this, [this](float){ update(); });
    connect(scene.get(), &Scene::ambientChanged, this, [this](float v){
        m_lights.setAmbient(m_lights.ambientColor(), v);
        update();
    });
}

Viewport::~Viewport()
{
    makeCurrent();
    if (m_gizmoVao) glDeleteVertexArrays(1, &m_gizmoVao);
    if (m_gizmoVbo) glDeleteBuffers(1, &m_gizmoVbo);
    doneCurrent();
}

// ── OpenGL ─────────────────────────────────────────────────────────────────────

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();
    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();
    setupGizmoShader();
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

    // Gizmo overlay
    if (m_gizmoMode != GizmoMode::None && m_scene->selectedIndex() >= 0)
        drawGizmoOverlay();
}

// ── Gizmo Setup ────────────────────────────────────────────────────────────────

void Viewport::setupGizmoShader()
{
    m_gizmoShader = std::make_unique<QOpenGLShaderProgram>();
    m_gizmoShader->addShaderFromSourceCode(QOpenGLShader::Vertex,   GIZMO_VERT);
    m_gizmoShader->addShaderFromSourceCode(QOpenGLShader::Fragment, GIZMO_FRAG);
    m_gizmoShader->link();

    glGenVertexArrays(1, &m_gizmoVao);
    glGenBuffers(1, &m_gizmoVbo);
    glBindVertexArray(m_gizmoVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVbo);
    // Pre-allocate for 2 vertices (line)
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ── Gizmo Draw ─────────────────────────────────────────────────────────────────

void Viewport::drawAxisArrow(const QVector3D &origin, const QVector3D &axisDir,
                              const QVector4D &color, float len)
{
    // Upload 2 vertices: origin → origin + axisDir*len
    QVector3D tip = origin + axisDir * len;
    float verts[6] = { origin.x(), origin.y(), origin.z(),
                       tip.x(),    tip.y(),    tip.z() };

    glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_gizmoShader->setUniformValue("uColor", color);

    glBindVertexArray(m_gizmoVao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Viewport::drawGizmoOverlay()
{
    auto mdl = m_scene->selectedModel();
    if (!mdl) return;

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;
    QMatrix4x4 proj = m_camera.projectionMatrix(aspect);
    QMatrix4x4 view = m_camera.viewMatrix();
    QMatrix4x4 vp   = proj * view;

    // Gizmo size: fixed screen size (scale by distance)
    float dist = (m_camera.position() - mdl->position).length();
    float gLen = dist * 0.22f;  // 모델까지 거리의 22%

    m_gizmoShader->bind();
    m_gizmoShader->setUniformValue("uMVP", vp);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.0f);

    const QVector3D &pos = mdl->position;

    if (m_gizmoMode == GizmoMode::Translate || m_gizmoMode == GizmoMode::Scale) {
        // X (빨강)
        drawAxisArrow(pos, {1,0,0}, {1.0f, 0.15f, 0.15f, 1.0f}, gLen);
        // Y (초록)
        drawAxisArrow(pos, {0,1,0}, {0.15f, 1.0f, 0.15f, 1.0f}, gLen);
        // Z (파랑)
        drawAxisArrow(pos, {0,0,1}, {0.15f, 0.4f, 1.0f, 1.0f},  gLen);
    } else if (m_gizmoMode == GizmoMode::Rotate) {
        // Rotate: draw circle-ish lines with 12 segments per axis
        // X axis ring (YZ plane)
        const int SEG = 16;
        for (int i = 0; i < SEG; ++i) {
            float a0 = i     * 2.f * M_PI / SEG;
            float a1 = (i+1) * 2.f * M_PI / SEG;
            QVector3D p0 = pos + QVector3D(0, std::cos(a0), std::sin(a0)) * gLen;
            QVector3D p1 = pos + QVector3D(0, std::cos(a1), std::sin(a1)) * gLen;
            float verts[6] = {p0.x(),p0.y(),p0.z(), p1.x(),p1.y(),p1.z()};
            glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            m_gizmoShader->setUniformValue("uColor", QVector4D(1.0f,0.15f,0.15f,1.0f));
            glBindVertexArray(m_gizmoVao); glDrawArrays(GL_LINES, 0, 2); glBindVertexArray(0);

            QVector3D q0 = pos + QVector3D(std::cos(a0), 0, std::sin(a0)) * gLen;
            QVector3D q1 = pos + QVector3D(std::cos(a1), 0, std::sin(a1)) * gLen;
            float verts2[6] = {q0.x(),q0.y(),q0.z(), q1.x(),q1.y(),q1.z()};
            glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts2), verts2);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            m_gizmoShader->setUniformValue("uColor", QVector4D(0.15f,1.0f,0.15f,1.0f));
            glBindVertexArray(m_gizmoVao); glDrawArrays(GL_LINES, 0, 2); glBindVertexArray(0);

            QVector3D r0 = pos + QVector3D(std::cos(a0), std::sin(a0), 0) * gLen;
            QVector3D r1 = pos + QVector3D(std::cos(a1), std::sin(a1), 0) * gLen;
            float verts3[6] = {r0.x(),r0.y(),r0.z(), r1.x(),r1.y(),r1.z()};
            glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts3), verts3);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            m_gizmoShader->setUniformValue("uColor", QVector4D(0.15f,0.4f,1.0f,1.0f));
            glBindVertexArray(m_gizmoVao); glDrawArrays(GL_LINES, 0, 2); glBindVertexArray(0);
        }
    }

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    m_gizmoShader->release();
}

// ── Screenshot — 깜빡임 없이 캡처 ────────────────────────────────────────────────

void Viewport::takeScreenshot(const QString &outputPath)
{
    // makeCurrent 보장 후 grabFramebuffer
    makeCurrent();
    QImage img = grabFramebuffer();
    doneCurrent();
    img.save(outputPath, "PNG");
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
    // 현재 ambient 강도 유지
    m_lights.setAmbient(m_lights.ambientColor(), m_scene->ambientStrength());
    update();
}

void Viewport::setBackgroundColor(const QColor &c) { m_scene->setBackgroundColor(c); update(); }
QColor Viewport::backgroundColor() const           { return m_scene->backgroundColor(); }
void Viewport::setGizmoMode(GizmoMode mode)         { m_gizmoMode = mode; update(); }

// ── Picking ────────────────────────────────────────────────────────────────────

void Viewport::screenToRay(const QPoint &p, QVector3D &ro, QVector3D &rd) const
{
    float ndcX =  (2.f * p.x() / width())  - 1.f;
    float ndcY = -(2.f * p.y() / height()) + 1.f;

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;
    QMatrix4x4 invVP = (m_camera.projectionMatrix(aspect) * m_camera.viewMatrix()).inverted();

    QVector4D n4 = invVP * QVector4D(ndcX, ndcY, -1.f, 1.f);
    QVector4D f4 = invVP * QVector4D(ndcX, ndcY,  1.f, 1.f);
    QVector3D nearPt = QVector3D(n4) / n4.w();
    QVector3D farPt  = QVector3D(f4) / f4.w();
    ro = nearPt;
    rd = (farPt - nearPt).normalized();
}

static bool raySphereIntersect(const QVector3D &ro, const QVector3D &rd,
                                const QVector3D &c, float r, float &tOut)
{
    QVector3D oc = ro - c;
    float b = QVector3D::dotProduct(oc, rd);
    float disc = b*b - (QVector3D::dotProduct(oc,oc) - r*r);
    if (disc < 0.f) return false;
    float t = -b - std::sqrt(disc);
    if (t < 0.f) t = -b + std::sqrt(disc);
    if (t < 0.f) return false;
    tOut = t;
    return true;
}

int Viewport::pickModel(const QPoint &pos)
{
    QVector3D ro, rd;
    screenToRay(pos, ro, rd);

    const auto &models = m_scene->models();
    float bestT = std::numeric_limits<float>::max();
    int   bestI = -1;

    for (int i = 0; i < (int)models.size(); ++i) {
        const auto &mdl = *models[i];
        if (!mdl.visible) continue;
        float radius = std::max({mdl.scale.x(), mdl.scale.y(), mdl.scale.z()}) * 1.8f;
        radius = std::max(radius, 0.5f);
        float t;
        if (raySphereIntersect(ro, rd, mdl.position, radius, t)) {
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

    QVector3D forward = (m_camera.target() - m_camera.position()).normalized();
    QVector3D right   = QVector3D::crossProduct(forward, {0,1,0}).normalized();
    QVector3D up      = QVector3D::crossProduct(right, forward).normalized();

    float dist = (m_camera.position() - mdl->position).length();
    float sens = dist * 0.003f + 0.005f;

    float dx =  delta.x() * sens;
    float dy = -delta.y() * sens;

    switch (m_gizmoMode) {
    case GizmoMode::Translate:
        mdl->position += right * dx + up * dy;
        break;
    case GizmoMode::Rotate:
        mdl->rotation += QVector3D(dy * 50.f, dx * 50.f, 0.f);
        break;
    case GizmoMode::Scale: {
        float s = 1.f + (dx + dy);
        mdl->scale *= std::max(0.01f, s);
        break;
    }
    default: break;
    }
}

// ── Input ──────────────────────────────────────────────────────────────────────

void Viewport::mousePressEvent(QMouseEvent *e)
{
    m_lastMousePos  = e->pos();
    m_activeButton  = e->button();
    m_draggingGizmo = false;
    setFocus();

    if (e->button() == Qt::LeftButton) {
        int sel = m_scene->selectedIndex();

        // Gizmo drag — 선택된 모델의 화면 투영 중심 근처 클릭 시
        if (sel >= 0 && m_gizmoMode != GizmoMode::None) {
            const auto &mdl = *m_scene->models()[sel];
            float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;
            QMatrix4x4 vp = m_camera.projectionMatrix(aspect) * m_camera.viewMatrix();
            QVector4D clip = vp * QVector4D(mdl.position, 1.f);
            if (clip.w() > 0.f) {
                QVector3D ndc = QVector3D(clip) / clip.w();
                int sx = static_cast<int>((ndc.x() + 1.f) * 0.5f * width());
                int sy = static_cast<int>((1.f - ndc.y()) * 0.5f * height());
                int d  = (e->pos() - QPoint(sx, sy)).manhattanLength();

                // Gizmo 반경: 거리 비례
                float dist = (m_camera.position() - mdl.position).length();
                float screenRadius = dist * 0.22f * height() / (2.f * dist * std::tan(qDegreesToRadians(m_camera.fov() * 0.5f)));
                if (d < static_cast<int>(screenRadius) + 40) {
                    m_draggingGizmo = true;
                    return;
                }
            }
        }

        // 일반 픽킹
        int hit = pickModel(e->pos());
        m_scene->setSelectedIndex(hit);
        emit modelSelected(hit);
    }
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint delta = e->pos() - m_lastMousePos;
    m_lastMousePos = e->pos();

    if (m_activeButton == Qt::LeftButton) {
        if (m_draggingGizmo && m_gizmoMode != GizmoMode::None) {
            applyGizmoDrag(delta);
        } else {
            m_camera.orbit(delta.x() * 0.4f, delta.y() * 0.4f);
        }
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
    // R = 카메라 리셋 (기즈모 없음)
    // Q/W/E/T = 기즈모 모드 (Scale은 R 대신 T)
    int newGizmo = -1;
    switch (e->key()) {
    case Qt::Key_R:     resetCamera();                          break;
    case Qt::Key_F:     onModelAdded();                        break;
    case Qt::Key_Space: toggleAnimation();                      break;
    case Qt::Key_Q:     newGizmo = 0; break;
    case Qt::Key_W:     newGizmo = 1; break;
    case Qt::Key_E:     newGizmo = 2; break;
    case Qt::Key_T:     newGizmo = 3; break;   // Scale (R 충돌 방지)
    default: QOpenGLWidget::keyPressEvent(e); return;
    }
    if (newGizmo >= 0) {
        m_gizmoMode = static_cast<GizmoMode>(newGizmo);
        emit gizmoModeChangedByKey(newGizmo);
        update();
    }
}
