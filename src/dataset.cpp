#include "dataset.h"

#include <QDebug>

#include "csv.hpp"

using namespace std;

void WaterQualityDataset::loadData(const string& filename) {
  try {
    csv::CSVReader reader(filename);

    data.clear();

    for (const auto& row : reader) {
      QualitySample sample{
          row["@id"].get<>(),
          row["sample.samplingPoint.notation"].get<>(),
          row["sample.samplingPoint.label"].get<>(),
          row["sample.sampleDateTime"].get<>(),
          row["determinand.label"].get<>(),
          row["determinand.definition"].get<>(),
          row["determinand.notation"].get<>(),
          row["result"].get<>(),
          row["determinand.unit.label"].get<>(),
          row["sample.sampledMaterialType.label"].get<>(),
          row["sample.purpose.label"].get<>(),
          row["sample.samplingPoint.easting"].get<>(),
          row["sample.samplingPoint.northing"].get<>()};
      data.push_back(sample);
    }

    qDebug() << "Loaded" << data.size() << "rows from" << QString::fromStdString(filename);

  } catch (const std::exception& e) {
    qWarning() << "Error loading CSV:" << e.what();
    throw;
  }
}