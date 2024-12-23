#pragma once

#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include <QtCharts>

#include "basepage.h"
#include "dataset.h"
#include "model.h"

class LitterPage : public BasePage {
  Q_OBJECT

 public:
  LitterPage(QWidget *parent = nullptr);
  void loadDataset(const QString &filename) override;

 private slots:
  void updateCharts();
  void onWaterTypeFilterChanged(const QString &type);
  void onLocationSelected(const QString &location);
  void onLocationSearchChanged(const QString &text);
  void onBarHovered(bool status, int index, QBarSet *barset);

 private:
  void setupUI() override;
  void setupCharts();
  void setupFilters();
  void loadData();
  void updateLocationCompleter();
  void setupInfoPanel();

  SampleModel model;
  QStringList allLocations;

  QFrame *controlsFrame;
  QHBoxLayout *controlsLayout;
  QComboBox *waterTypeFilter;
  QLineEdit *locationSearch;
  QCompleter *locationCompleter;

  QChartView *locationBarChart;
  QChart *barChart;

  QVBoxLayout *mainLayout;
  QFrame *infoPanel;
  QHBoxLayout *chartsLayout;

  std::map<QString, std::pair<double, double>> locationLevels;

  QLineSeries *warningThresholdLine;      // 警告阈值线
  QLineSeries *dangerThresholdLine;       // 危险阈值线
  const double WARNING_THRESHOLD = 15.0;  // 警告阈值
  const double DANGER_THRESHOLD = 25.0;   // 危险阈值
};