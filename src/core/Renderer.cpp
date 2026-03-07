#include "Renderer.h"
#include "Scene.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <QMatrix3x3>
#include <cstring>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// GLSL Sources
// ─────────────────────────────────────────────────────────────────────────────

static const char *PBR_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in vec3 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMat;

out vec3 vPos;
out vec3 vNormal;
out vec2 vUV;
out vec3 vTangent;

void main(){
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vPos    = worldPos.xyz;
    vNormal = normalize(uNormalMat * aNormal);
    vTangent= normalize(uNormalMat * aTangent);
    vUV     = aUV;
    gl_Position = uProj * uView * worldPos;
}
)GLSL";

static const char *PBR_FRAG = R"GLSL(
#version 410 core
in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;

out vec4 fragColor;

uniform vec3  uCamPos;
uniform vec3  uBaseColor;
uniform float uRoughness;
uniform float uMetallic;

uniform int   uLightCount;
uniform vec3  uLightDir[4];
uniform vec3  uLightColor[4];
uniform float uLightIntensity[4];
uniform int   uLightType[4];

uniform vec3  uAmbient;
uniform float uAmbientStr;

// Texture / wireframe mode:  0 = normal PBR,  1 = solid (no texture)
uniform int   uRenderMode;
// Selection highlight overlay
uniform float uSelectionHighlight;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N,vec3 H,float r){
    float a=r*r; float a2=a*a;
    float d=max(dot(N,H),0.0); float d2=d*d;
    float denom=d2*(a2-1.0)+1.0;
    return a2/(PI*denom*denom+0.0001);
}
float GeometrySchlick(float d,float r){
    float k=(r+1.0)*(r+1.0)/8.0;
    return d/(d*(1.0-k)+k);
}
float GeometrySmith(vec3 N,vec3 V,vec3 L,float r){
    return GeometrySchlick(max(dot(N,V),0.0),r)*GeometrySchlick(max(dot(N,L),0.0),r);
}
vec3 fresnelSchlick(float c,vec3 F0){ return F0+(1.0-F0)*pow(clamp(1.0-c,0.0,1.0),5.0); }

void main(){
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCamPos - vPos);

    vec3 albedo;
    if (uRenderMode == 1) {
        // Solid / no-texture: use a neutral grey
        albedo = pow(vec3(0.72), vec3(2.2));
    } else {
        albedo = pow(uBaseColor, vec3(2.2));
    }

    float metal = uMetallic;
    float rough = max(uRoughness, 0.04);

    vec3 F0 = mix(vec3(0.04), albedo, metal);
    vec3 Lo = vec3(0.0);

    for(int i=0;i<uLightCount;i++){
        vec3 L;
        float attn = 1.0;
        if(uLightType[i]==0){
            L = normalize(-uLightDir[i]);
        } else {
            vec3 toL = uLightDir[i] - vPos;
            float d  = length(toL);
            L    = toL/d;
            attn = 1.0/(d*d+0.0001);
        }
        vec3 H = normalize(V+L);
        vec3 rad = uLightColor[i] * uLightIntensity[i] * attn;

        float NDF = DistributionGGX(N,H,rough);
        float G   = GeometrySmith(N,V,L,rough);
        vec3  F   = fresnelSchlick(max(dot(H,V),0.0),F0);

        vec3 num   = NDF*G*F;
        float denom= 4.0*max(dot(N,V),0.0)*max(dot(N,L),0.0)+0.0001;
        vec3 spec  = num/denom;

        vec3 kD = (vec3(1.0)-F)*(1.0-metal);
        float NL= max(dot(N,L),0.0);
        Lo += (kD*albedo/PI + spec)*rad*NL;
    }

    vec3 ambient = uAmbient * uAmbientStr * albedo;
    vec3 color   = ambient + Lo;
    color = color/(color+vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    // Selection tint: blend with golden yellow
    if (uSelectionHighlight > 0.0) {
        vec3 selColor = vec3(1.0, 0.75, 0.1);
        color = mix(color, selColor, uSelectionHighlight * 0.35);
    }

    fragColor = vec4(color,1.0);
}
)GLSL";

// ── Wireframe overlay shader ──────────────────────────────────────────────────
static const char *WIRE_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP;
void main(){ gl_Position = uMVP * vec4(aPos,1.0); }
)GLSL";

static const char *WIRE_FRAG = R"GLSL(
#version 410 core
out vec4 fragColor;
uniform vec4 uColor;
void main(){ fragColor = uColor; }
)GLSL";

static const char *GRID_VERT = R"GLSL(
#version 410 core
layout(location=0) in vec3 aPos;
uniform mat4 uVP;
out float vDist;
void main(){
    vDist = length(aPos.xz);
    gl_Position = uVP * vec4(aPos,1.0);
}
)GLSL";

static const char *GRID_FRAG = R"GLSL(
#version 410 core
in float vDist;
out vec4 fragColor;
void main(){
    float alpha = 1.0 - smoothstep(30.0, 60.0, vDist);
    fragColor = vec4(0.4, 0.4, 0.5, alpha * 0.35);
}
)GLSL";

// ─────────────────────────────────────────────────────────────────────────────

Renderer::Renderer()  = default;
Renderer::~Renderer()
{
    if (m_gridVao) glDeleteVertexArrays(1, &m_gridVao);
    if (m_gridVbo) glDeleteBuffers(1, &m_gridVbo);
}

void Renderer::initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupShaders();

    // Build grid geometry
    std::vector<float> gridVerts;
    const float ext = 100.f;
    const int   N   = 40;
    for (int i = -N; i <= N; ++i) {
        float p = i * (ext / N);
        gridVerts.insert(gridVerts.end(), {-ext, 0, p,  ext, 0, p});
        gridVerts.insert(gridVerts.end(), {p, 0, -ext,  p, 0, ext});
    }

    glGenVertexArrays(1, &m_gridVao);
    glGenBuffers(1, &m_gridVbo);
    glBindVertexArray(m_gridVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(gridVerts.size()*sizeof(float)),
                 gridVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::setupShaders()
{
    m_pbr = std::make_unique<QOpenGLShaderProgram>();
    m_pbr->addShaderFromSourceCode(QOpenGLShader::Vertex,   PBR_VERT);
    m_pbr->addShaderFromSourceCode(QOpenGLShader::Fragment, PBR_FRAG);
    m_pbr->link();

    m_wire = std::make_unique<QOpenGLShaderProgram>();
    m_wire->addShaderFromSourceCode(QOpenGLShader::Vertex,   WIRE_VERT);
    m_wire->addShaderFromSourceCode(QOpenGLShader::Fragment, WIRE_FRAG);
    m_wire->link();

    m_grid = std::make_unique<QOpenGLShaderProgram>();
    m_grid->addShaderFromSourceCode(QOpenGLShader::Vertex,   GRID_VERT);
    m_grid->addShaderFromSourceCode(QOpenGLShader::Fragment, GRID_FRAG);
    m_grid->link();
}

// ── Render frame ──────────────────────────────────────────────────────────────

void Renderer::render(const std::shared_ptr<Scene> &scene,
                       const QMatrix4x4 &view,
                       const QMatrix4x4 &proj,
                       const QVector3D  &camPos,
                       const LightSystem &lights,
                       double /*animTime*/)
{
    const QMatrix4x4 vp = proj * view;

    // Draw grid first
    m_grid->bind();
    m_grid->setUniformValue("uVP", vp);
    glBindVertexArray(m_gridVao);
    glDrawArrays(GL_LINES, 0, (40*2+1)*4);
    glBindVertexArray(0);
    m_grid->release();

    // Render mode: 0 = textured/PBR, 1 = solid grey
    const int renderMode = scene->textureVisible() ? 0 : 1;
    const float lightMult = scene->lightIntensityMultiplier();
    const int selectedIdx = scene->selectedIndex();

    m_pbr->bind();
    m_pbr->setUniformValue("uCamPos",    camPos);
    m_pbr->setUniformValue("uRenderMode", renderMode);

    const QColor amb = lights.ambientColor();
    m_pbr->setUniformValue("uAmbient",
        QVector3D(amb.redF(), amb.greenF(), amb.blueF()));
    m_pbr->setUniformValue("uAmbientStr", lights.ambientStrength());

    bindLights(lights, lightMult);

    // Wireframe mode: draw solid then wireframe on top
    if (!scene->textureVisible()) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.f, 1.f);
    }

    int modelIdx = 0;
    for (const auto &model : scene->models()) {
        if (!model->visible) { ++modelIdx; continue; }

        if (m_gpuModels.find(model->assimpScene) == m_gpuModels.end())
            uploadModel(*model);

        QMatrix4x4 modelMat;
        modelMat.translate(model->position);
        modelMat.rotate(model->rotation.x(), 1,0,0);
        modelMat.rotate(model->rotation.y(), 0,1,0);
        modelMat.rotate(model->rotation.z(), 0,0,1);
        modelMat.scale(model->scale);

        QMatrix3x3 normalMat = modelMat.normalMatrix();
        m_pbr->setUniformValue("uModel",     modelMat);
        m_pbr->setUniformValue("uView",      view);
        m_pbr->setUniformValue("uProj",      proj);
        m_pbr->setUniformValue("uNormalMat", normalMat);
        m_pbr->setUniformValue("uSelectionHighlight",
            (modelIdx == selectedIdx) ? 1.0f : 0.0f);

        auto it = m_gpuModels.find(model->assimpScene);
        if (it != m_gpuModels.end()) {
            for (const auto &mesh : it->second.meshes) {
                m_pbr->setUniformValue("uBaseColor",
                    QVector3D(mesh.baseColorR, mesh.baseColorG, mesh.baseColorB));
                m_pbr->setUniformValue("uRoughness", mesh.roughness);
                m_pbr->setUniformValue("uMetallic",  mesh.metallic);
                glBindVertexArray(mesh.vao);
                glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
            }
        }
        ++modelIdx;
    }
    glBindVertexArray(0);
    m_pbr->release();

    if (!scene->textureVisible()) {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // Wireframe overlay pass
    if (!scene->textureVisible()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_wire->bind();
        modelIdx = 0;
        for (const auto &model : scene->models()) {
            if (!model->visible) { ++modelIdx; continue; }

            QMatrix4x4 modelMat;
            modelMat.translate(model->position);
            modelMat.rotate(model->rotation.x(), 1,0,0);
            modelMat.rotate(model->rotation.y(), 0,1,0);
            modelMat.rotate(model->rotation.z(), 0,0,1);
            modelMat.scale(model->scale);

            m_wire->setUniformValue("uMVP", vp * modelMat);
            bool selected = (modelIdx == selectedIdx);
            m_wire->setUniformValue("uColor",
                selected ? QVector4D(1.f, 0.75f, 0.1f, 1.f)
                         : QVector4D(0.4f, 0.7f, 1.f, 0.8f));

            auto it = m_gpuModels.find(model->assimpScene);
            if (it != m_gpuModels.end()) {
                for (const auto &mesh : it->second.meshes) {
                    glBindVertexArray(mesh.vao);
                    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
                }
            }
            ++modelIdx;
        }
        glBindVertexArray(0);
        m_wire->release();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::bindLights(const LightSystem &lights, float intensityMult)
{
    const auto &ls = lights.lights();
    int n = static_cast<int>(std::min(ls.size(), size_t(4)));
    m_pbr->setUniformValue("uLightCount", n);
    for (int i = 0; i < n; ++i) {
        const auto &l = ls[i];
        QString d  = QString("uLightDir[%1]").arg(i);
        QString c  = QString("uLightColor[%1]").arg(i);
        QString in = QString("uLightIntensity[%1]").arg(i);
        QString tp = QString("uLightType[%1]").arg(i);
        QVector3D dir = (l.type == Light::Point) ? l.position : l.direction.normalized();
        m_pbr->setUniformValue(d.toLatin1(), dir);
        m_pbr->setUniformValue(c.toLatin1(),
            QVector3D(l.color.redF(), l.color.greenF(), l.color.blueF()));
        m_pbr->setUniformValue(in.toLatin1(), l.intensity * intensityMult);
        m_pbr->setUniformValue(tp.toLatin1(), (int)l.type);
    }
}

// ── GPU upload / delete ────────────────────────────────────────────────────────

void Renderer::uploadModel(SceneModel &model)
{
    const aiScene *sc = static_cast<const aiScene*>(model.assimpScene);
    if (!sc) return;
    GpuModel gpu;
    uploadMeshes(gpu, sc, sc->mRootNode);
    m_gpuModels[model.assimpScene] = std::move(gpu);
}

void Renderer::uploadMeshes(GpuModel &gpu, const aiScene *sc, const aiNode *node)
{
    for (unsigned i = 0; i < node->mNumMeshes; ++i)
        gpu.meshes.push_back(uploadMesh(sc->mMeshes[node->mMeshes[i]], sc));
    for (unsigned i = 0; i < node->mNumChildren; ++i)
        uploadMeshes(gpu, sc, node->mChildren[i]);
}

GpuMesh Renderer::uploadMesh(const aiMesh *m, const aiScene *sc)
{
    struct Vertex { float x,y,z, nx,ny,nz, u,v, tx,ty,tz; };
    std::vector<Vertex>   verts(m->mNumVertices);
    std::vector<unsigned> idx;

    for (unsigned i = 0; i < m->mNumVertices; ++i) {
        auto &v = verts[i];
        v.x=m->mVertices[i].x; v.y=m->mVertices[i].y; v.z=m->mVertices[i].z;
        if (m->HasNormals()) {
            v.nx=m->mNormals[i].x; v.ny=m->mNormals[i].y; v.nz=m->mNormals[i].z;
        }
        if (m->mTextureCoords[0]) {
            v.u=m->mTextureCoords[0][i].x; v.v=m->mTextureCoords[0][i].y;
        }
        if (m->HasTangentsAndBitangents()) {
            v.tx=m->mTangents[i].x; v.ty=m->mTangents[i].y; v.tz=m->mTangents[i].z;
        }
    }
    for (unsigned i = 0; i < m->mNumFaces; ++i)
        for (unsigned j = 0; j < m->mFaces[i].mNumIndices; ++j)
            idx.push_back(m->mFaces[i].mIndices[j]);

    GpuMesh gm;
    gm.indexCount = static_cast<int>(idx.size());

    if (m->mMaterialIndex < sc->mNumMaterials) {
        aiMaterial *mat = sc->mMaterials[m->mMaterialIndex];
        aiColor4D col;
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, col) == AI_SUCCESS) {
            gm.baseColorR = col.r; gm.baseColorG = col.g; gm.baseColorB = col.b;
        }
        float rough = 0.5f, metal = 0.f;
        mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, rough);
        mat->Get(AI_MATKEY_METALLIC_FACTOR,  metal);
        gm.roughness = rough; gm.metallic = metal;
    }

    glGenVertexArrays(1, &gm.vao);
    glGenBuffers(1, &gm.vbo);
    glGenBuffers(1, &gm.ebo);
    glBindVertexArray(gm.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gm.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size()*sizeof(Vertex)),
                 verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gm.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(idx.size()*sizeof(unsigned)),
                 idx.data(), GL_STATIC_DRAW);

    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);        glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*4));    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*4));    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8*4));    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return gm;
}

void Renderer::deleteModel(SceneModel &model)
{
    auto it = m_gpuModels.find(model.assimpScene);
    if (it == m_gpuModels.end()) return;
    for (auto &mesh : it->second.meshes) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }
    m_gpuModels.erase(it);
}
