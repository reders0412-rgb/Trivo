#include "ScenePanel.h"
#include "../core/Scene.h"
#include "../utils/IconProvider.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QHeaderView>
#include <QFrame>

ScenePanel::ScenePanel(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // ── Scene list tree ───────────────────────────────────────────────────────
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    m_tree->setRootIsDecorated(false);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setMinimumHeight(100);
    m_tree->setMaximumHeight(160);
    layout->addWidget(m_tree);

    auto *removeBtn = new QPushButton("🗑  모델 제거 / Remove", this);
    layout->addWidget(removeBtn);
    connect(removeBtn, &QPushButton::clicked, this, &ScenePanel::onRemoveSelected);

    // ── Model info ────────────────────────────────────────────────────────────
    auto *infoFrame = new QFrame(this);
    infoFrame->setFrameShape(QFrame::StyledPanel);
    auto *infoLayout = new QVBoxLayout(infoFrame);
    infoLayout->setContentsMargins(8, 6, 8, 6);

    m_infoLabel = new QLabel("모델을 선택하세요 / Select a model", infoFrame);
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setStyleSheet("font-size: 11px; line-height: 1.5;");
    infoLayout->addWidget(m_infoLabel);
    layout->addWidget(infoFrame);

    // ── Transform group ───────────────────────────────────────────────────────
    m_xformGroup = new QGroupBox("변환 / Transform", this);
    auto *xformLayout = new QVBoxLayout(m_xformGroup);
    xformLayout->setSpacing(4);

    auto makeRow = [&](const QString &label, QDoubleSpinBox **spins,
                       double minV, double maxV, double step, int dec) {
        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(label, m_xformGroup);
        lbl->setFixedWidth(46);
        lbl->setStyleSheet("font-size:11px; font-weight:bold;");
        row->addWidget(lbl);
        const char *axes[] = {"X","Y","Z"};
        const char *colors[] = {"#c0392b","#27ae60","#2980b9"};
        for (int i=0; i<3; ++i) {
            auto *axLabel = new QLabel(axes[i], m_xformGroup);
            axLabel->setFixedWidth(12);
            axLabel->setStyleSheet(
                QString("color:%1; font-size:10px; font-weight:bold;").arg(colors[i]));
            row->addWidget(axLabel);
            spins[i] = new QDoubleSpinBox(m_xformGroup);
            spins[i]->setRange(minV, maxV);
            spins[i]->setSingleStep(step);
            spins[i]->setDecimals(dec);
            spins[i]->setMinimumWidth(60);
            row->addWidget(spins[i]);
            connect(spins[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this, &ScenePanel::onTransformChanged);
        }
        xformLayout->addLayout(row);
    };

    makeRow("위치/Pos", m_pos, -99999, 99999, 0.1, 3);
    makeRow("회전/Rot", m_rot, -360,   360,   1.0, 1);
    makeRow("크기/Scl", m_scl, 0.001,  9999,  0.1, 3);

    m_visible = new QCheckBox("표시 / Visible", m_xformGroup);
    m_visible->setChecked(true);
    xformLayout->addWidget(m_visible);
    connect(m_visible, &QCheckBox::toggled, this, &ScenePanel::onTransformChanged);

    m_xformGroup->setEnabled(false);
    layout->addWidget(m_xformGroup);
    layout->addStretch();

    connect(m_tree, &QTreeWidget::itemClicked, this, &ScenePanel::onModelSelected);

    // Sync from scene's selection signal (e.g. viewport click)
    connect(m_scene.get(), &Scene::selectionChanged, this, [this](int idx){
        syncSelection(idx);
    });
}

void ScenePanel::refresh()
{
    populateTree();
}

void ScenePanel::populateTree()
{
    m_tree->clear();
    const auto &models = m_scene->models();
    for (int i=0; i<(int)models.size(); ++i) {
        const auto &m = models[i];
        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, m->name);
        item->setIcon(0, IconProvider::forExtensionPath(m->filePath));
        item->setData(0, Qt::UserRole, i);
    }
    // Re-select current
    if (m_selected >= 0 && m_selected < m_tree->topLevelItemCount())
        m_tree->setCurrentItem(m_tree->topLevelItem(m_selected));
}

void ScenePanel::syncSelection(int index)
{
    // Called when viewport picks a model
    if (index == m_selected) return;
    m_selected = index;

    m_tree->blockSignals(true);
    m_tree->clearSelection();
    if (index >= 0 && index < m_tree->topLevelItemCount())
        m_tree->setCurrentItem(m_tree->topLevelItem(index));
    m_tree->blockSignals(false);

    selectModel(index);
}

void ScenePanel::onModelSelected(QTreeWidgetItem *item, int)
{
    if (!item) return;
    int idx = item->data(0, Qt::UserRole).toInt();
    m_scene->setSelectedIndex(idx);  // will emit selectionChanged → syncSelection
}

void ScenePanel::selectModel(int index)
{
    const auto &models = m_scene->models();
    if (index < 0 || index >= (int)models.size()) {
        m_xformGroup->setEnabled(false);
        m_selected = -1;
        m_infoLabel->setText("모델을 선택하세요 / Select a model");
        return;
    }
    m_selected = index;
    m_xformGroup->setEnabled(true);

    m_updating = true;
    const auto &mdl = *models[index];
    m_pos[0]->setValue(mdl.position.x());
    m_pos[1]->setValue(mdl.position.y());
    m_pos[2]->setValue(mdl.position.z());
    m_rot[0]->setValue(mdl.rotation.x());
    m_rot[1]->setValue(mdl.rotation.y());
    m_rot[2]->setValue(mdl.rotation.z());
    m_scl[0]->setValue(mdl.scale.x());
    m_scl[1]->setValue(mdl.scale.y());
    m_scl[2]->setValue(mdl.scale.z());
    m_visible->setChecked(mdl.visible);
    m_updating = false;

    updateInfoLabel(index);
}

void ScenePanel::updateInfoLabel(int index)
{
    const auto &models = m_scene->models();
    if (index < 0 || index >= (int)models.size()) return;
    const auto &mdl = *models[index];

    QFileInfo fi(mdl.filePath);
    qint64 bytes = fi.size();
    QString sizeStr;
    if      (bytes > 1024*1024) sizeStr = QString("%1 MB").arg(bytes/1024.0/1024.0, 0,'f',1);
    else if (bytes > 1024)      sizeStr = QString("%1 KB").arg(bytes/1024.0, 0,'f',1);
    else                        sizeStr = QString("%1 B").arg(bytes);

    QString animStr = mdl.hasAnim ? "  🎬 애니메이션 있음" : "";

    m_infoLabel->setText(
        QString(
            "<b>%1</b><br>"
            "📁 %2 &nbsp;│&nbsp; 💾 %3<br>"
            "▲ 버텍스 <b>%4</b> &nbsp; △ 삼각형 <b>%5</b><br>"
            "🔷 메시 <b>%6</b> &nbsp; 🎨 재질 <b>%7</b> &nbsp; 🦴 본 <b>%8</b>%9"
        )
        .arg(mdl.name)
        .arg(fi.suffix().toUpper())
        .arg(sizeStr)
        .arg(mdl.vertCount)
        .arg(mdl.triCount)
        .arg(mdl.meshCount)
        .arg(mdl.matCount)
        .arg(mdl.boneCount)
        .arg(animStr)
    );
}

void ScenePanel::onTransformChanged()
{
    if (m_updating || m_selected < 0) return;
    const auto &models = m_scene->models();
    if (m_selected >= (int)models.size()) return;
    auto &mdl = *models[m_selected];
    mdl.position = {(float)m_pos[0]->value(),(float)m_pos[1]->value(),(float)m_pos[2]->value()};
    mdl.rotation = {(float)m_rot[0]->value(),(float)m_rot[1]->value(),(float)m_rot[2]->value()};
    mdl.scale    = {(float)m_scl[0]->value(),(float)m_scl[1]->value(),(float)m_scl[2]->value()};
    mdl.visible  = m_visible->isChecked();
}

void ScenePanel::onRemoveSelected()
{
    if (m_selected < 0) return;
    m_scene->removeModel(m_selected);
    m_selected = -1;
    m_xformGroup->setEnabled(false);
    m_infoLabel->setText("모델을 선택하세요 / Select a model");
    populateTree();
}
