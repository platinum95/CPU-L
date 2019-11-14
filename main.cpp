#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QObject>
#include "myclass.h"
#include "cpu-l-lib/gfxinfo.h"

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

    GfxInfo gfxInfo;
    gfxInfo.populateInfo();

    mainObject->findChild< QObject * >( "glDriverText" )->setProperty( "text", QVariant( gfxInfo.glCoreInfo.versionString.c_str() ) );
    mainObject->findChild< QObject * >( "vkDriverText" )->setProperty( "text", QVariant( gfxInfo.vkDevProperties[ 1 ].vkDeviceDriverProps.driverInfo ) );
    mainObject->findChild< QObject * >( "kernelText" )->setProperty( "text", QVariant( gfxInfo.kernelInfo.release ) );

    //engine.rootContext()->setContextProperty( "_petersCoolClass", mclass );

    return app.exec();
}
