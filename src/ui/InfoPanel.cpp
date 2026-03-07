#include "InfoPanel.h"
#include "../core/Scene.h"
#include "../utils/FileUtils.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QFileInfo>

InfoPanel::InfoPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 2, 8, 2);
    m_label = new QLabel(this);
    m_label->setStyleSheet("font-size: 11px;");
    layout->addWidget(m_label);
    clear();
}

void InfoPanel::clear()
{
    m_label->setText("Trivo  —  파일을 열거나 드래그하세요  /  Open or drag a 3D file here");
}

void InfoPanel::refresh(std::shared_ptr<SceneModel> model)
{
    if (!model) { clear(); return; }

    QFileInfo fi(model->filePath);
    const QString size = FileUtils::humanSize(fi.size());
    const QString fmt  = FileUtils::formatName(fi.suffix().toLower());
    const QString anim = model->hasAnim ? "  🎬" : "";

    m_label->setText(
        QString("📄 %1  │  %2  │  %3  │  "
                "버텍스 %4  삼각형 %5  메시 %6  재질 %7  본 %8%9")
            .arg(model->name)
            .arg(fmt)
            .arg(size)
            .arg(model->vertCount)
            .arg(model->triCount)
            .arg(model->meshCount)
            .arg(model->matCount)
            .arg(model->boneCount)
            .arg(anim)
    );
}
