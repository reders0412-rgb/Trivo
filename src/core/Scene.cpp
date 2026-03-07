#include "Scene.h"
#include "ModelLoader.h"
#include <QFileInfo>
#include <QVector3D>

Scene::Scene(QObject *parent) : QObject(parent) {}

void Scene::addModel(const QString &path)
{
    auto model = ModelLoader::load(path);
    if (!model) return;

    // Offset each added model slightly so they don't overlap
    float offset = static_cast<float>(m_models.size()) * 1.5f;
    model->position = QVector3D(offset, 0.f, 0.f);

    m_models.push_back(model);
    emit modelAdded(model);
}

void Scene::clear()
{
    m_models.clear();
    emit sceneCleared();
}

void Scene::removeModel(int index)
{
    if (index < 0 || index >= static_cast<int>(m_models.size())) return;
    m_models.erase(m_models.begin() + index);
    emit modelRemoved(index);
}
