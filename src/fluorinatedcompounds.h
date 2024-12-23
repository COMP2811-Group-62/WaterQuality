#pragma once

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
  void setupUI() override;
  void configureHeader(QVBoxLayout* header);
  void configureMap(QVBoxLayout* column);
  void configureSidebar(QVBoxLayout* column);

  SampleModel* model;

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

  QComboBox* locationSelector;
  QComboBox* pollutantSelector;
  QComboBox* timeRangeSelector;
};