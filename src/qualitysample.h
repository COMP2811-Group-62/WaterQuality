#pragma once

#include <iostream>
#include <string>

class QualitySample {
 public:
  // Constructor with all fields
  QualitySample(const std::string& id,
                const std::string& samplingPointNotation = "",
                const std::string& samplingPointLabel = "",
                const std::string& samplingPointDateTime = "",
                const std::string& determinandLabel = "",
                const std::string& determinandDefinition = "",
                const std::string& determinandNotation = "",
                const std::string& result = "",
                const std::string& determinandUnitLabel = "",
                const std::string& sampledMaterialType = "",
                const std::string& purposeLabel = "",
                const std::string& easting = "",
                const std::string& northing = "");

  // Getters
  std::string getId() const { return id; }
  std::string getSamplingPointNotation() const { return samplingPointNotation; }
  std::string getSamplingPointLabel() const { return samplingPointLabel; }
  std::string getSamplingPointDateTime() const { return samplingPointDateTime; }
  std::string getDeterminandLabel() const { return determinandLabel; }
  std::string getDeterminandDefinition() const { return determinandDefinition; }
  std::string getDeterminandNotation() const { return determinandNotation; }
  std::string getResult() const { return result; }
  std::string getDeterminandUnitLabel() const { return determinandUnitLabel; }
  std::string getSampledMaterialType() const { return sampledMaterialType; }
  std::string getPurposeLabel() const { return purposeLabel; }
  std::string getEasting() const { return easting; }
  std::string getNorthing() const { return northing; }

 private:
  std::string id;
  std::string samplingPointNotation;
  std::string samplingPointLabel;
  std::string samplingPointDateTime;
  std::string determinandLabel;
  std::string determinandDefinition;
  std::string determinandNotation;
  std::string result;
  std::string determinandUnitLabel;
  std::string sampledMaterialType;
  std::string purposeLabel;
  std::string easting;
  std::string northing;
};

std::ostream& operator<<(std::ostream&, const QualitySample&);