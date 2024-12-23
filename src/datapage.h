#pragma once

#include "basepage.h"
#include "model.h"

class QTableView;
class QHBoxLayout;

class DataPage : public BasePage {
  Q_OBJECT

 public:
  DataPage(QWidget* parent = nullptr);
  void loadDataset(const QString& filename) override;
  int getRowCount() const {
    return model.rowCount(QModelIndex());
  }

 private:
  void setupUI() override;

  SampleModel model;
  QTableView* table;
  QHBoxLayout* pageLayout;
};