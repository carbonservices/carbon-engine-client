#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtWebEngineQuick>
#include <QDir>
#include "logging/Logger.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {
void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static Logger qtLog("Qt");
    switch (type) {
    case QtDebugMsg:
        qtLog.debug(msg);
        break;
    case QtInfoMsg:
        qtLog.info(msg);
        break;
    case QtWarningMsg:
        qtLog.warning(msg);
        break;
    case QtCriticalMsg:
        qtLog.critical(msg);
        break;
    case QtFatalMsg:
        qtLog.critical(QStringLiteral("FATAL: ") + msg);
        abort();
    }
}
} // namespace

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    qInstallMessageHandler(qtMessageHandler);

    QtWebEngineQuick::initialize();

    QGuiApplication app(argc, argv);
    app.setApplicationName("Carbon ESG");
    app.setOrganizationName("Carbon Services");
    app.setApplicationDisplayName("Carbon ESG");
#ifdef CARBON_ENGINE_VERSION
    app.setApplicationVersion(CARBON_ENGINE_VERSION);
#else
    app.setApplicationVersion("1.0.0");
#endif

    Logger log("Client");
    log.info("Starting Carbon ESG Client v" + app.applicationVersion());

    QQuickStyle::setStyle("Basic");

    QIcon appIcon(":/CarbonEngineClient/logo.png");
    if (appIcon.isNull())
    {
        appIcon = QIcon(":/qt/qml/CarbonEngineClient/logo.png");
    }
    if (appIcon.isNull())
    {
        appIcon = QIcon(QDir(QCoreApplication::applicationDirPath()).filePath("logo.png"));
    }
    app.setWindowIcon(appIcon);

    QQmlApplicationEngine engine;

    // Add import paths for build dir & bin dir
    engine.addImportPath(QCoreApplication::applicationDirPath());
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/../CarbonEngineClient");

    log.debug("QML Import paths: " + engine.importPathList().join(", "));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, [&log](const QUrl &url) {
            log.critical("Object creation failed for: " + url.toString());
#ifdef Q_OS_WIN
            QString errMsg = "Failed to create QML component:\n" + url.toString() + "\n\nSee logs in Documents/CarbonESG/logs for details.";
            MessageBoxW(NULL, reinterpret_cast<LPCWSTR>(errMsg.utf16()), L"Carbon ESG Startup Error", MB_OK | MB_ICONERROR);
#endif
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.loadFromModule("CarbonEngineClient", "Main");

    if (engine.rootObjects().isEmpty())
    {
        log.warning("rootObjects is empty from module, attempting QRC fallback...");
        QUrl qrcUrl("qrc:/qt/qml/CarbonEngineClient/Main.qml");
        engine.load(qrcUrl);
        if (engine.rootObjects().isEmpty()) {
            log.critical("Failed to load UI (CarbonEngineClient.Main).");
#ifdef Q_OS_WIN
            MessageBoxW(NULL, L"Failed to load UI (CarbonEngineClient.Main).\n\nPlease verify that 'CarbonEngineClient' directory exists alongside the executable.", L"Carbon ESG Startup Error", MB_OK | MB_ICONERROR);
#endif
            return -1;
        }
    }

    log.info("Main window loaded successfully. Executing application loop.");
    return app.exec();
}