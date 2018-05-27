#include "ae2.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    Ae2 timer;
    QQmlContext* ctx=engine.rootContext();
    ctx->setContextProperty("benimHaberciReferansi",&timer);


    return app.exec();

}
