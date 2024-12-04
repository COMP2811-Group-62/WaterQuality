#pragma once
#include "qualitysample.h"
#include <string>
#include <vector>

class WaterQualityDataset {
public:
    void loadData(const std::string& filename);
    void checkDataExists() const;
    size_t size() const { return data.size(); }
    const std::vector<QualitySample>& getData() const { return data; }
    
    // Array access operators
    const QualitySample& operator[](size_t index) const { return data[index]; }
    QualitySample& operator[](size_t index) { return data[index]; }

private:
    std::vector<QualitySample> data;
};