#include "IconProvider.h"
#include <QFileInfo>
#include <QMap>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFont>

// ─────────────────────────────────────────────────────────────────────────────
// ICO 리소스 경로 매핑 (resources.qrc 기반)
// ─────────────────────────────────────────────────────────────────────────────

static const QMap<QString, QString> &icoMap()
{
    static const QMap<QString, QString> m {
        {"glb",   ":/icons/ext_glb.ico"},
        {"gltf",  ":/icons/ext_gltf.ico"},
        {"fbx",   ":/icons/ext_fbx.ico"},
        {"obj",   ":/icons/ext_obj.ico"},
        {"dae",   ":/icons/ext_dae.ico"},
        {"3ds",   ":/icons/ext_3ds.ico"},
        {"stl",   ":/icons/ext_stl.ico"},
        {"ply",   ":/icons/ext_ply.ico"},
        {"blend", ":/icons/ext_blend.ico"},
        {"bvh",   ":/icons/ext_bvh.ico"},
        {"x",     ":/icons/ext_x.ico"},
        {"dxf",   ":/icons/ext_dxf.ico"},
        {"ifc",   ":/icons/ext_ifc.ico"},
        {"step",  ":/icons/ext_step.ico"},
        {"stp",   ":/icons/ext_step.ico"},
        {"iges",  ":/icons/ext_iges.ico"},
        {"igs",   ":/icons/ext_iges.ico"},
    };
    return m;
}

// ─────────────────────────────────────────────────────────────────────────────
// Fallback: 프로그래매틱 배지 아이콘 (ICO 없는 확장자용)
// ─────────────────────────────────────────────────────────────────────────────

static const QMap<QString, unsigned int> &colorMap()
{
    static const QMap<QString, unsigned int> m {
        {"glb",   0xFF3B82F6}, {"gltf",  0xFF60A5FA},
        {"fbx",   0xFFEF4444}, {"x",     0xFFDC2626},
        {"md2",   0xFFB91C1C}, {"md3",   0xFFB91C1C},
        {"bvh",   0xFFF97316}, {"smd",   0xFFFB923C}, {"vta",   0xFFFDBA74},
        {"obj",   0xFF10B981}, {"dae",   0xFF059669}, {"3ds",   0xFF047857},
        {"ply",   0xFF34D399}, {"stl",   0xFF6EE7B7},
        {"blend", 0xFFEC4899}, {"lwo",   0xFFDB2777}, {"ms3d",  0xFFF472B6},
        {"step",  0xFF8B5CF6}, {"stp",   0xFF8B5CF6}, {"iges",  0xFF7C3AED},
        {"igs",   0xFF7C3AED}, {"ifc",   0xFF6D28D9}, {"dxf",   0xFFA78BFA},
        {"pcd",   0xFFF59E0B}, {"xyz",   0xFFD97706}, {"pts",   0xFFB45309},
    };
    return m;
}

static QIcon makeBadgeIcon(const QString &label, unsigned int bgColor)
{
    const int SZ = 64;
    QPixmap px(SZ, SZ);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    QColor bg(bgColor);
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(2, 2, SZ-4, SZ-4, 10, 10);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(0,0,0,50), 1.5));
    p.drawRoundedRect(2, 2, SZ-4, SZ-4, 10, 10);
    p.setPen(Qt::white);
    int fs = label.length() > 3 ? 9 : 12;
    QFont font("Arial", fs, QFont::Bold);
    p.setFont(font);
    p.drawText(QRect(2, 2, SZ-4, SZ-4), Qt::AlignCenter, label.toUpper());
    p.end();
    return QIcon(px);
}

// ─────────────────────────────────────────────────────────────────────────────

QIcon IconProvider::forExtension(const QString &ext)
{
    // 1. ICO 리소스 우선 사용
    if (icoMap().contains(ext)) {
        QIcon ic(icoMap()[ext]);
        if (!ic.isNull()) return ic;
    }
    // 2. Fallback: 프로그래매틱 배지
    const unsigned int col = colorMap().value(ext, 0xFF6B7280);
    return makeBadgeIcon(ext.isEmpty() ? "3D" : ext, col);
}

QIcon IconProvider::forExtensionPath(const QString &filePath)
{
    return forExtension(QFileInfo(filePath).suffix().toLower());
}

// ─────────────────────────────────────────────────────────────────────────────
// App icons (ICO 리소스 → fallback 순)
// ─────────────────────────────────────────────────────────────────────────────

static QIcon loadIco(const QString &icoPath, const QString &fallbackEmoji)
{
    QIcon ic(icoPath);
    if (!ic.isNull()) return ic;
    // Emoji fallback
    QPixmap px(32, 32);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setFont(QFont("Segoe UI Emoji", 20));
    p.drawText(QRect(0,0,32,32), Qt::AlignCenter, fallbackEmoji);
    p.end();
    return QIcon(px);
}

QIcon IconProvider::appIcon()        { return loadIco(":/icons/trivo_app.ico", "🧊"); }
QIcon IconProvider::openIcon()       { return loadIco(":/icons/open.ico",      "📂"); }
QIcon IconProvider::addIcon()        { return loadIco(":/icons/add.ico",       "➕"); }
QIcon IconProvider::resetIcon()      { return loadIco(":/icons/reset.ico",     "🔄"); }
QIcon IconProvider::playIcon()       { return loadIco(":/icons/play.ico",      "▶"); }
QIcon IconProvider::pauseIcon()      { return loadIco(":/icons/pause.ico",     "⏸"); }
QIcon IconProvider::screenshotIcon() { return loadIco(":/icons/camera.ico",    "📷"); }
QIcon IconProvider::darkThemeIcon()  { return loadIco(":/icons/moon.ico",      "🌙"); }
QIcon IconProvider::lightThemeIcon() { return loadIco(":/icons/sun.ico",       "☀"); }
