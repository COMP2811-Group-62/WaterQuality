#include "styles.h"

#include <QFile>
#include <QTextStream>
#include <QWidget>

QString Styles::loadStyleSheet(const QString& path) {
  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    qWarning() << "Could not load stylesheet:" << path;
    return QString();
  }
  QTextStream in(&file);
  return in.readAll();
}

QString Styles::combineStyleSheets(const QStringList& stylesheetPaths) {
  QStringList styles;
  for (const QString& path : stylesheetPaths) {
    styles << loadStyleSheet(path);
  }
  return styles.join("\n");
}
