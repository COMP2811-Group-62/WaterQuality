#pragma once

#include <QApplication>
#include <QLocale>
#include <QObject>
#include <QSettings>
#include <QTranslator>

class LanguageManager : public QObject {
  Q_OBJECT
 public:
  static LanguageManager& instance() {
    static LanguageManager instance;
    return instance;
  }

  void initialize() {
    QString locale = QLocale::system().name();
    currentTranslator = std::make_unique<QTranslator>();
    if (currentTranslator->load("aquawatch_" + locale.split("_")[0])) {
      QApplication::installTranslator(currentTranslator.get());
      updateLayoutDirection(locale);
    }
  }

 private:
  void updateLayoutDirection(const QString& locale) {
    // Set RTL for Arabic
    if (locale.startsWith("ar")) {
      QApplication::setLayoutDirection(Qt::RightToLeft);
    } else {
      QApplication::setLayoutDirection(Qt::LeftToRight);
    }
  }

  LanguageManager() = default;
  std::unique_ptr<QTranslator> currentTranslator;
};