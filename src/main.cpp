#include <QApplication>
#include <QSurfaceFormat>
#include <QDir>
#include <QIcon>
#include <QFile>
#include "core/MainWindow.h"
#include "utils/IconProvider.h"

int main(int argc, char *argv[])
{
    // ── OpenGL surface format ─────────────────────────────────────────────────
    QSurfaceFormat fmt;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSamples(8);
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setApplicationName("Trivo");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Trivo");

    // ICO 파일 우선, 없으면 fallback
    app.setWindowIcon(IconProvider::appIcon());

    // ── Main window ───────────────────────────────────────────────────────────
    MainWindow w;
    w.show();

    // ── CLI: open file if passed ──────────────────────────────────────────────
    const QStringList args = app.arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (QFile::exists(args[i])) {
            w.openFile(args[i]);
            break;
        }
    }

    return app.exec();
}
