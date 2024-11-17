#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}