#pragma once

#include "basepage.h"
#include "model.h"

class QTableView;
class QHBoxLayout;

class DataPage : public BasePage {
  Q_OBJECT

 public:
  DataPage(SampleModel* model, QWidget* parent = nullptr);
  void refreshView() override;

 private:
  void setupUI() override;

  SampleModel* model;
  QTableView* table;
  QHBoxLayout* pageLayout;
};