#include "IconProvider.h"
#include <QFileInfo>
#include <QMap>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QColor>

// ─────────────────────────────────────────────────────────────────────────────
// Programmatic badge icon generation
// ─────────────────────────────────────────────────────────────────────────────

QIcon IconProvider::makeBadgeIcon(const QString &label, unsigned int bgColor)
{
    const int SZ = 48;
    QPixmap px(SZ, SZ);
    px.fill(Qt::transparent);

    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    QColor bg(bgColor);
    // Rounded rect background
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(2, 2, SZ-4, SZ-4, 8, 8);

    // Dark border
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(0,0,0,60), 1));
    p.drawRoundedRect(2, 2, SZ-4, SZ-4, 8, 8);

    // Text
    p.setPen(Qt::white);
    QFont font("Arial", label.length() > 3 ? 7 : 9, QFont::Bold);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    p.setFont(font);
    p.drawText(QRect(2, 2, SZ-4, SZ-4), Qt::AlignCenter, label.toUpper());

    p.end();
    return QIcon(px);
}

// Extension → icon colour map
QIcon IconProvider::forExtension(const QString &ext)
{
    struct ExtInfo { unsigned int color; };
    static const QMap<QString, ExtInfo> map {
        // Modern
        {"glb",   {0xFF3B82F6}},
        {"gltf",  {0xFF60A5FA}},
        // Game / animation
        {"fbx",   {0xFFEF4444}},
        {"x",     {0xFFDC2626}},
        {"md2",   {0xFFB91C1C}},
        {"md3",   {0xFFB91C1C}},
        {"bvh",   {0xFFF97316}},
        {"smd",   {0xFFFB923C}},
        {"vta",   {0xFFFDBA74}},
        // General
        {"obj",   {0xFF10B981}},
        {"dae",   {0xFF059669}},
        {"3ds",   {0xFF047857}},
        {"ply",   {0xFF34D399}},
        {"stl",   {0xFF6EE7B7}},
        // DCC
        {"blend", {0xFFEC4899}},
        {"lwo",   {0xFFDB2777}},
        {"lws",   {0xFFBE185D}},
        {"ms3d",  {0xFFF472B6}},
        // CAD
        {"step",  {0xFF8B5CF6}},
        {"stp",   {0xFF8B5CF6}},
        {"iges",  {0xFF7C3AED}},
        {"igs",   {0xFF7C3AED}},
        {"ifc",   {0xFF6D28D9}},
        {"dxf",   {0xFFA78BFA}},
        // Point cloud
        {"pcd",   {0xFFF59E0B}},
        {"xyz",   {0xFFD97706}},
        {"pts",   {0xFFB45309}},
    };

    if (map.contains(ext)) {
        return makeBadgeIcon(ext, map[ext].color);
    }
    // Generic grey
    return makeBadgeIcon(ext.isEmpty() ? "3D" : ext, 0xFF6B7280);
}

QIcon IconProvider::forExtensionPath(const QString &filePath)
{
    return forExtension(QFileInfo(filePath).suffix().toLower());
}

// ─────────────────────────────────────────────────────────────────────────────
// App icons — SVG resources with pixmap fallback
// ─────────────────────────────────────────────────────────────────────────────

static QIcon svgOrFallback(const QString &svgPath, const QString &emoji,
                            unsigned int fallbackColor = 0xFF3A7BD5)
{
    QIcon ic(svgPath);
    if (!ic.isNull()) return ic;

    // Fallback: emoji/text badge
    QPixmap px(32, 32);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    p.setFont(QFont("Segoe UI Emoji", 18));
    p.drawText(QRect(0,0,32,32), Qt::AlignCenter, emoji);
    p.end();
    return QIcon(px);
}

QIcon IconProvider::appIcon()        { return svgOrFallback(":/icons/trivo_app.svg",  "🧊"); }
QIcon IconProvider::openIcon()       { return svgOrFallback(":/icons/open.svg",       "📂"); }
QIcon IconProvider::addIcon()        { return svgOrFallback(":/icons/add.svg",        "➕"); }
QIcon IconProvider::resetIcon()      { return svgOrFallback(":/icons/reset.svg",      "🔄"); }
QIcon IconProvider::playIcon()       { return svgOrFallback(":/icons/play.svg",       "▶"); }
QIcon IconProvider::pauseIcon()      { return svgOrFallback(":/icons/pause.svg",      "⏸"); }
QIcon IconProvider::screenshotIcon() { return svgOrFallback(":/icons/camera.svg",     "📷"); }
QIcon IconProvider::darkThemeIcon()  { return svgOrFallback(":/icons/moon.svg",       "🌙"); }
QIcon IconProvider::lightThemeIcon() { return svgOrFallback(":/icons/sun.svg",        "☀"); }
