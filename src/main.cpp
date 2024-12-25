#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

#include "mainwindow.h"
#include "styles.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QString styleSheet = Styles::loadStyleSheet(":/styles/main.qss");
  if (!styleSheet.isEmpty()) {
    app.setStyleSheet(styleSheet);
    qDebug() << "Applied stylesheet successfully";
  } else {
    qDebug() << "Failed to load stylesheet!";
  }

  MainWindow window;
  window.show();
  return app.exec();
}