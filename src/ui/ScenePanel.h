#pragma once
#include <QWidget>
#include <memory>

class Scene;
class QTreeWidget;
class QTreeWidgetItem;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;
class QLabel;

class ScenePanel : public QWidget
{
    Q_OBJECT
public:
    explicit ScenePanel(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);
    void refresh();

    // Called by Viewport when a model is selected by clicking in the scene
    void syncSelection(int index);

private slots:
    void onModelSelected(QTreeWidgetItem *item, int col);
    void onRemoveSelected();
    void onTransformChanged();

private:
    void buildTransformUI(QGroupBox *group);
    void populateTree();
    void selectModel(int index);
    void updateInfoLabel(int index);

    std::shared_ptr<Scene> m_scene;
    QTreeWidget  *m_tree       = nullptr;
    QGroupBox    *m_xformGroup = nullptr;
    QDoubleSpinBox *m_pos[3] = {}, *m_rot[3] = {}, *m_scl[3] = {};
    QCheckBox    *m_visible    = nullptr;
    QLabel       *m_infoLabel  = nullptr;
    int           m_selected   = -1;
    bool          m_updating   = false;
};
