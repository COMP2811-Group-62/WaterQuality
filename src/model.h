#pragma once

#include <QAbstractTableModel>

#include "dataset.h"

class SampleModel : public QAbstractTableModel {
 public:
  SampleModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}
  void updateFromFile(const QString&);
  bool hasData() const { return dataset.size() > 0; }

  int rowCount(const QModelIndex& index) const { return dataset.size(); }
  int columnCount(const QModelIndex& index) const { return 13; }
  QVariant data(const QModelIndex&, int) const;
  QVariant headerData(int, Qt::Orientation, int) const;

 private:
  WaterQualityDataset dataset;
};
