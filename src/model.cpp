#include "model.h"

void SampleModel::updateFromFile(const QString& filename) {
  beginResetModel();
  dataset.loadData(filename.toStdString());
  endResetModel();
}

QVariant SampleModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft | Qt::AlignVCenter);
  } else if (role == Qt::DisplayRole) {
    QualitySample q = dataset[index.row()];
    switch (index.column()) {
      case 0:
        return QString::fromStdString(q.getId());
      case 1:
        return QString::fromStdString(q.getSamplingPointNotation());
      case 2:
        return QString::fromStdString(q.getSamplingPointLabel());
      case 3:
        return QString::fromStdString(q.getSamplingPointDateTime());
      case 4:
        return QString::fromStdString(q.getDeterminandLabel());
      case 5:
        return QString::fromStdString(q.getDeterminandDefinition());
      case 6:
        return QString::fromStdString(q.getDeterminandNotation());
      case 7:
        return QString::fromStdString(q.getResult());
      case 8:
        return QString::fromStdString(q.getDeterminandUnitLabel());
      case 9:
        return QString::fromStdString(q.getSampledMaterialType());
      case 10:
        return QString::fromStdString(q.getPurposeLabel());
      case 11:
        return QString::fromStdString(q.getEasting());
      case 12:
        return QString::fromStdString(q.getNorthing());
      default:
        return QVariant();
    }
  }

  return QVariant();
}

QVariant SampleModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (orientation == Qt::Vertical) {
    return QVariant(section + 1);
  }

  switch (section) {
    case 0:
      return QString("ID");
    case 1:
      return QString("Sampling Point Notation");
    case 2:
      return QString("Sampling Point Label");
    case 3:
      return QString("Sample DateTime");
    case 4:
      return QString("Determinand Label");
    case 5:
      return QString("Determinand Definition");
    case 6:
      return QString("Determinand Notation");
    case 7:
      return QString("Result");
    case 8:
      return QString("Determinand Unit Label");
    case 9:
      return QString("Sampled Material Type");
    case 10:
      return QString("Purpose Label");
    case 11:
      return QString("Easting");
    case 12:
      return QString("Northing");
    default:
      return QVariant();
  }
}
