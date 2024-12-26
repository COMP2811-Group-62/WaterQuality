#pragma once

#include <QQuickWidget>

#include "basepage.h"
#include "model.h"

class QHBoxLayout;
class QTableView;
class QComboBox;
class QChart;

class FluorinatedCompounds : public BasePage {
  Q_OBJECT

 public:
  FluorinatedCompounds(SampleModel* model, QWidget* parent = nullptr);
  void refreshView() override;

 private:
  // to store points which fit the correct prefix for PFAS
  struct dataPoint
  {
    QString dataURL;
    QString location;
    QString pollutant;
    QString date;
    QString result;
    QString units;
  };

  QMap<QString, QString> months = {
    {"January 2024", "2024-01"},
    {"February 2024", "2024-02"},
    {"March 2024", "2024-03"},
    {"April 2024", "2024-04"},
    {"May 2024", "2024-05"},
    {"June 2024", "2024-06"},
    {"July 2024", "2024-07"},
    {"August 2024", "2024-08"},
    {"September 2024", "2024-09"},
    {"October 2024", "2024-10"},
    {"November 2024", "2024-11"},
    {"December 2024", "2024-12"},
    {"All points", "2024"}};

  void setupUI() override;
  void configureHeader(QVBoxLayout* header);
  void configureMap(QVBoxLayout* column);
  void configureSidebar(QVBoxLayout* column);
  void findPollutants();
  void addMapCirlces();
  void onLocationChange();
  void clearMap();

  SampleModel* model;
  
  // page componenents
  QVBoxLayout* header;
  QHBoxLayout* body;
  QVBoxLayout* page;
  QVBoxLayout* columnLeft;
  QVBoxLayout* columnRight;

  QVBoxLayout* headerInner;
  QVBoxLayout* sidbarInnerHeader;
  QVBoxLayout* sidbarInnerBody;
  QHBoxLayout* mapControls;
  QVBoxLayout* headerLables;

  // header combination boxes
  QComboBox* locationSelector;
  QComboBox* pollutantSelector;
  QComboBox* timeRangeSelector;

  // polutant and location lists
  QList<QString> filteredPolutants;
  QList<QString> filteredLocations;
  
  // list of points
  QList<dataPoint> dataPoints;

  QQuickWidget* mapView;
};