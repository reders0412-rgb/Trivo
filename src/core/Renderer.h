#pragma once
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>
#include <memory>
#include <unordered_map>
#include "LightSystem.h"

class Scene;
struct SceneModel;
struct aiScene;
struct aiMesh;
struct aiNode;

struct GpuMesh {
    unsigned int vao = 0, vbo = 0, ebo = 0;
    int indexCount = 0;
    float baseColorR = 0.8f, baseColorG = 0.8f, baseColorB = 0.8f;
    float roughness  = 0.5f;
    float metallic   = 0.0f;
};

struct GpuModel {
    std::vector<GpuMesh> meshes;
};

class Renderer : protected QOpenGLFunctions_4_1_Core
{
public:
    Renderer();
    ~Renderer();

    void initialize();
    void render(const std::shared_ptr<Scene> &scene,
                const QMatrix4x4 &view,
                const QMatrix4x4 &proj,
                const QVector3D  &camPos,
                const LightSystem &lights,
                double animTime);

    void uploadModel(SceneModel &model);
    void deleteModel(SceneModel &model);

private:
    void uploadMeshes(GpuModel &gpu, const aiScene *sc, const aiNode *node);
    GpuMesh uploadMesh(const aiMesh *m, const aiScene *sc);
    void setupShaders();
    void bindLights(const LightSystem &lights, float intensityMult);
    void drawGrid(const QMatrix4x4 &vp);

    std::unique_ptr<QOpenGLShaderProgram> m_pbr;
    std::unique_ptr<QOpenGLShaderProgram> m_grid;
    std::unique_ptr<QOpenGLShaderProgram> m_wire;  // wireframe / outline

    std::unordered_map<void*, GpuModel> m_gpuModels;

    unsigned int m_gridVao = 0, m_gridVbo = 0;
};
