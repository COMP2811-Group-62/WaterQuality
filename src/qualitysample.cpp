#include "qualitysample.h"

using namespace std;

QualitySample::QualitySample(const string& id,
                             const string& samplingPointNotation,
                             const string& samplingPointLabel,
                             const string& samplingPointDateTime,
                             const string& determinandLabel,
                             const string& determinandDefinition,
                             const string& determinandNotation,
                             const string& result,
                             const string& determinandUnitLabel,
                             const string& sampledMaterialType,
                             const string& purposeLabel,
                             const string& easting,
                             const string& northing)
    : id(id),
      samplingPointNotation(samplingPointNotation),
      samplingPointLabel(samplingPointLabel),
      samplingPointDateTime(samplingPointDateTime),
      determinandLabel(determinandLabel),
      determinandDefinition(determinandDefinition),
      determinandNotation(determinandNotation),
      result(result),
      determinandUnitLabel(determinandUnitLabel),
      sampledMaterialType(sampledMaterialType),
      purposeLabel(purposeLabel),
      easting(easting),
      northing(northing) {}

ostream& operator<<(ostream& out, const QualitySample& qualitySample) {
  out << "id: " << qualitySample.getId() << "\n"
      << "Sampling Point Notation: " << qualitySample.getSamplingPointNotation() << "\n"
      << "Sampling Point Label: " << qualitySample.getSamplingPointLabel() << "\n"
      << "Sampling Point DateTime: " << qualitySample.getSamplingPointDateTime() << "\n"
      << "Determinand Label: " << qualitySample.getDeterminandLabel() << "\n"
      << "Determinand Definition: " << qualitySample.getDeterminandDefinition() << "\n"
      << "Determinand Notation: " << qualitySample.getDeterminandNotation() << "\n"
      << "Result: " << qualitySample.getResult() << "\n"
      << "Determinand Unit Label: " << qualitySample.getDeterminandUnitLabel() << "\n"
      << "Sampled Material Type: " << qualitySample.getSampledMaterialType() << "\n"
      << "Purpose Label: " << qualitySample.getPurposeLabel() << "\n"
      << "Easting: " << qualitySample.getEasting() << "\n"
      << "Northing: " << qualitySample.getNorthing();
  return out;
}
