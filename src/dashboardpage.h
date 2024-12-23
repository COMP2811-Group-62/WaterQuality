#pragma once

#include <QtCharts>

#include "basepage.h"
#include "model.h"

class QGridLayout;
class QFrame;
class QPushButton;

class DashboardPage : public BasePage {
  Q_OBJECT

 public:
  DashboardPage(SampleModel* model, QWidget* parent = nullptr);
  void refreshView() override;

 private slots:
  void navigateToPage(int pageIndex);

 signals:
  void pageChangeRequested(int index);

 private:
  struct PollutantMetrics {
    double currentLevel;
    double averageLevel;
    double maxLevel;
    int samplesAboveLimit;
    int totalSamples;
    QString mostCommon;
    QString trend;
  };

  // Store analyzed data
  PollutantMetrics commonPollutantMetrics;
  PollutantMetrics popsMetrics;
  PollutantMetrics litterMetrics;
  PollutantMetrics pfasMetrics;
  struct {
    double overallCompliance;
    int sitesCompliant;
    int totalSites;
    QString mainConcern;
    int sitesNeedingAction;
  } complianceMetrics;

  void setupUI() override;
  QFrame* createPollutantsCard();
  QFrame* createPOPsCard();
  QFrame* createLitterCard();
  QFrame* createFluorinatedCard();
  QFrame* createComplianceCard();

  void processData();
  QString getComplianceStatus(double value, double warningThreshold, double dangerThreshold);
  QColor getStatusColor(double value, double warningThreshold, double dangerThreshold);
  QString calculateTrend(const QVector<double>& values);
  double calculateCompliance(const QVector<double>& values, double threshold);

  QGridLayout* cardsLayout;
  SampleModel* model;
  QComboBox* timeRangeFilter;
  QComboBox* regionFilter;

  // Constants for thresholds
  const double POLLUTANTS_WARNING = 5.0;
  const double POLLUTANTS_DANGER = 7.0;
  const double POPS_WARNING = 0.005;
  const double POPS_DANGER = 0.007;
  const double LITTER_WARNING = 15.0;
  const double LITTER_DANGER = 25.0;
  const double PFAS_WARNING = 0.1;
  const double PFAS_DANGER = 0.2;
};