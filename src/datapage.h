#pragma once

#include "basepage.h"
#include "model.h"

class QTableView;
class QHBoxLayout;

class DataPage : public BasePage {
 public:
  DataPage(QWidget* parent = nullptr);

 private:
  void setupUI() override;

  SampleModel model;
  QTableView* table;
  QHBoxLayout* pageLayout;
};