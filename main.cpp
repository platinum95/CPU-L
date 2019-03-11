#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QObject>
#include "myclass.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    auto mainContext = engine.rootContext();
    auto mainObject = engine.rootObjects()[ 0 ];
    auto stackView = mainObject->findChild< QObject* >( "mainStackView" );
    MyClass * mclass = new MyClass( stackView );

    //engine.rootContext()->setContextProperty( "_petersCoolClass", mclass );

    return app.exec();
}
