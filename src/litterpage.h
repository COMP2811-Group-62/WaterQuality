#pragma once

#include <QComboBox>
#include <QtCharts>
#include "basepage.h"
#include "dataset.h"

class LitterPage : public BasePage {
    Q_OBJECT

public:
    LitterPage(QWidget* parent = nullptr);

private slots:
    void updateCharts();
    void onLocationFilterChanged(const QString& location);
    void onWaterTypeFilterChanged(const QString& type);

private:
    void setupUI() override;
    void setupCharts();
    void setupFilters();
    void loadData();

    WaterQualityDataset dataset;
    
    QChartView* locationBarChart;
    QChartView* waterTypePieChart;
    QChart* barChart;
    QChart* pieChart;
    
    QComboBox* locationFilter;
    QComboBox* waterTypeFilter;
    
    QVBoxLayout* mainLayout;
    QHBoxLayout* filtersLayout;
    QHBoxLayout* chartsLayout;
};
