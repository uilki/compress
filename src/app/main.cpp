#include "model/mainmodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(assetsPlugin)

int main(int argc, char* argv[]) {
  QGuiApplication       app(argc, argv);

  QQmlApplicationEngine engine;
  engine.addImportPath(":/");
  const QUrl url(u"qrc:/compress/Main.qml"_qs);
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  const char* workingDir = "";
  if (argc > 1)
    workingDir = argv[1];
  model::MainModel mainModel(workingDir);
  engine.rootContext()->setContextProperty("mainModel", &mainModel);
  engine.load(url);

  return app.exec();
}
