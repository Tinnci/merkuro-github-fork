#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "CalendarApp.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("PersonalCalendar"));
    QCoreApplication::setApplicationName(QStringLiteral("PersonalCalendar"));

    // Use Material style for a modern look without KDE dependencies
    qputenv("QT_QUICK_CONTROLS_STYLE", "Material");

    CalendarApp calendarApp;
    calendarApp.initialize();

    QQmlApplicationEngine engine;
    
    // Expose the controller to QML
    engine.rootContext()->setContextProperty(QStringLiteral("CalendarApp"), &calendarApp);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}
