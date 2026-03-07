#include "Scene.h"
#include "ModelLoader.h"
#include <QFileInfo>
#include <QVector3D>

Scene::Scene(QObject *parent) : QObject(parent) {}

void Scene::addModel(const QString &path)
{
    auto model = ModelLoader::load(path);
    if (!model) return;
    float offset = static_cast<float>(m_models.size()) * 1.5f;
    model->position = QVector3D(offset, 0.f, 0.f);
    m_models.push_back(model);
    emit modelAdded(model);
}

void Scene::clear()
{
    m_models.clear();
    m_selectedIndex = -1;
    emit sceneCleared();
}

void Scene::removeModel(int index)
{
    if (index < 0 || index >= static_cast<int>(m_models.size())) return;
    m_models.erase(m_models.begin() + index);
    if (m_selectedIndex == index)      m_selectedIndex = -1;
    else if (m_selectedIndex > index)  --m_selectedIndex;
    emit modelRemoved(index);
}

void Scene::setSelectedIndex(int i)
{
    if (i < -1 || i >= static_cast<int>(m_models.size())) i = -1;
    if (m_selectedIndex == i) return;
    m_selectedIndex = i;
    emit selectionChanged(i);
}

std::shared_ptr<SceneModel> Scene::selectedModel() const
{
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_models.size()))
        return nullptr;
    return m_models[m_selectedIndex];
}
