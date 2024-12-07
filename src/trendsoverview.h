#pragma once

#include "basepage.h"
#include "model.h"

class QHBoxLayout;
class QVBoxLayout;
class QChartView;
class QChart;
class QLineSeries;
class QLineEdit;
class QComboBox;
class QCompleter;
class QLabel;
class QValueAxis;
class QDateTimeAxis;
class QFrame;

class TrendsOverviewPage : public BasePage {
  Q_OBJECT

 public:
  explicit TrendsOverviewPage(QWidget* parent = nullptr);

 private slots:
  void onPollutantSelected(const QString& pollutant);
  void onLocationChanged(const QString& location);
  void onPointHovered(const QPointF& point, bool state);

 private:
  // UI Setup Methods
  void setupUI() override;
  void setupControlsSection(QVBoxLayout* parentLayout);
  void setupSearchControls(QHBoxLayout* layout);
  void setupStatsSection(QVBoxLayout* parentLayout);
  void setupChartSection(QVBoxLayout* parentLayout);
  void setupThresholdIndicators(QHBoxLayout* layout);

  // UI Helper Methods
  void addInfoCard(QHBoxLayout* layout, const QString& title, const QString& value);
  QFrame* createThresholdIndicator(const QString& label, const QString& range, const QString& objectName);

  // Chart Methods
  void setupChart();
  void updateChart();
  void configureAxes(const QMap<QDateTime, double>& dataPoints);
  void addSafetyThresholdLines();
  QString getComplianceStatus(double value) const;
  QColor getComplianceColor(double value) const;

  // Data Methods
  void populatePollutants();
  void updateLocations();
  void updateStats();
  QMap<QDateTime, double> collectChartData() const;

  // Widget Members
  QVBoxLayout* pageLayout{nullptr};
  QChartView* chartView{nullptr};
  QChart* chart{nullptr};
  QLineSeries* series{nullptr};
  QLineEdit* pollutantSearch{nullptr};
  QComboBox* locationSelector{nullptr};
  QCompleter* pollutantCompleter{nullptr};
  QValueAxis* axisY{nullptr};
  QDateTimeAxis* axisX{nullptr};
  QFrame* contentFrame{nullptr};

  // Data Members
  SampleModel model;
  QString currentPollutant;
  QString currentLocation;
  QStringList pollutants;

  // Constants
  static constexpr double SAFE_THRESHOLD = 5.0;
  static constexpr double WARNING_THRESHOLD = 7.0;
  static constexpr double AXIS_PADDING_FACTOR = 0.1;
  static const int MIN_CHART_HEIGHT = 400;
  static const int MAX_CONTROL_WIDTH = 300;
  static const int MAX_VISIBLE_ITEMS = 10;
};