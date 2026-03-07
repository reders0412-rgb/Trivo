#pragma once
#include <QIcon>
#include <QString>

class IconProvider
{
public:
    // Returns an appropriate icon for the file extension (generated programmatically)
    static QIcon forExtension(const QString &ext);
    static QIcon forExtensionPath(const QString &filePath);

    // App icons (SVG resource-based)
    static QIcon appIcon();
    static QIcon openIcon();
    static QIcon addIcon();
    static QIcon resetIcon();
    static QIcon playIcon();
    static QIcon pauseIcon();
    static QIcon screenshotIcon();
    static QIcon darkThemeIcon();
    static QIcon lightThemeIcon();

private:
    // Generates a coloured badge icon with text label
    static QIcon makeBadgeIcon(const QString &label, unsigned int bgColor);
};
