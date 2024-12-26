#pragma once
#include <QMap>
#include <QObject>
#include <QString>

class Compliance {
 public:
  static Compliance& instance() {
    static Compliance instance;
    return instance;
  }

  struct ThresholdInfo {
    double warningLevel;   // Warning threshold
    double criticalLevel;  // Critical/danger threshold
  };

  double getWarningThreshold(const QString& pollutant) const;
  double getCriticalThreshold(const QString& pollutant) const;
  bool isCompliant(const QString& pollutant, double value) const;

 private:
  Compliance() { initializeThresholds(); }
  void initializeThresholds();

  QMap<QString, ThresholdInfo> thresholds;
};