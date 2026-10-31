#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTimer>

#include "CalcBridge.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(QStringLiteral("ElektroRechner"));
    QGuiApplication::setApplicationName(QStringLiteral("ElektroRechner"));
    QGuiApplication::setApplicationVersion(QStringLiteral(ELC_APP_VERSION));
    QQuickStyle::setStyle(QStringLiteral("Material"));

    CalcBridge bridge;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("Calc"), &bridge);

    const QUrl url(QStringLiteral("qrc:/qt/qml/ElektroRechnerUi/qml/Main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    if (engine.rootObjects().isEmpty()) return -1;

    // Automated visual check: ELC_WINDOW_SIZE=1100x800, ELC_SCREENSHOT=file.png grabs the window and quits.
    if (const auto size = qEnvironmentVariable("ELC_WINDOW_SIZE").split('x'); size.size() == 2) {
        if (auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().constFirst())) {
            window->resize(size[0].toInt(), size[1].toInt());
        }
    }
    const QString shot = qEnvironmentVariable("ELC_SCREENSHOT");
    if (!shot.isEmpty()) {
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().constFirst());
        QTimer::singleShot(1500, &app, [window, shot] {
            if (window) window->grabWindow().save(shot);
            QCoreApplication::exit(0);
        });
    }
    return QGuiApplication::exec();
}
