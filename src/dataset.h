#pragma once

#include <vector>

#include "qualitysample.h"

class WaterQualityDataset {
 public:
  WaterQualityDataset() {}
  WaterQualityDataset(const std::string& filename) { loadData(filename); }
  void loadData(const std::string&);
  int size() const { return data.size(); }
  QualitySample operator[](int index) const { return data.at(index); }

 private:
  std::vector<QualitySample> data;
  void checkDataExists() const;
};
