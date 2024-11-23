#pragma once
#include <QString>

class QWidget;

class Styles {
 public:
  static QString loadStyleSheet(const QString& path);
  static QString combineStyleSheets(const QStringList& stylesheetPaths);
};