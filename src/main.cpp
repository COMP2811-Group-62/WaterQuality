#include <QApplication>
#include <QFile>
#include <QStyle>
#include <QTranslator>

#include "languagemanager.h"
#include "mainwindow.h"
#include "styles.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // Initialize language from saved settings or system locale
  LanguageManager::instance().initialize();

  app.setStyle("fusion");

  QString styleSheet = Styles::loadStyleSheet(":/styles/main.qss");
  if (!styleSheet.isEmpty()) {
    app.setStyleSheet(styleSheet);
  } else {
    qDebug() << "Failed to load stylesheet!";
  }

  MainWindow window;
  window.show();
  return app.exec();
}