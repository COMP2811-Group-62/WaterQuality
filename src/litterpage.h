#pragma once

#include <QComboBox>
#include <QtCharts>
#include "basepage.h"
#include "dataset.h"

class LitterPage : public BasePage
{
    Q_OBJECT

public:
    LitterPage(QWidget *parent = nullptr);

private slots:
    void updateCharts();
    void onLocationFilterChanged(const QString &location);
    void onWaterTypeFilterChanged(const QString &type);

private:
    void setupUI() override;
    void setupCharts();
    void setupFilters();
    void loadData();

    // 数据
    WaterQualityDataset dataset;

    // 控制区域组件
    QFrame *controlsFrame;
    QHBoxLayout *controlsLayout;
    QComboBox *locationFilter;
    QComboBox *waterTypeFilter;

    // 图表组件
    QChartView *locationBarChart;
    QChartView *waterTypePieChart;
    QChart *barChart;
    QChart *pieChart;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *chartsLayout;
};