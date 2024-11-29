#pragma once

#include "basepage.h"

class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QComboBox;
class QFrame;
class QPushButton;
class QChart;
class QLineSeries;
class QChartView;
class QValueAxis;

class POPsPage : public BasePage {
    Q_OBJECT

public:
    explicit POPsPage(QWidget* parent = nullptr);

private:
    void setupUI() override;
    void setupControls();
    void setupDataDisplay();
    void setupInfoPanel();
    void createDataSection();
    void createLegend();
    void setupChart();

    // Layout containers
    QVBoxLayout* dataLayout;
    QHBoxLayout* controlsLayout;
    QVBoxLayout* infoPanelLayout;
    
    // Control widgets
    QComboBox* pollutantSelector;
    QComboBox* timeRangeSelector;
    QPushButton* exportButton;
    
    // Display widgets
    QFrame* dataDisplayFrame;
    QFrame* legendFrame;
    QLabel* dataLabel;
    QChartView* chartView;
    QFrame* graphFrame;
    
    // Chart components
    QChart* chart;
    QLineSeries* currentLevelSeries;
    QLineSeries* warningThresholdSeries;
    QLineSeries* dangerThresholdSeries;
    QValueAxis* axisY;
    
    // Info panel widgets
    QFrame* infoPanel;
    QLabel* safetyLevelIndicator;
    QLabel* healthRiskLabel;
    QLabel* thresholdLabel;
    QLabel* trendLabel;

private slots:
    void updateDisplay(int index);
    void updateTimeRange(int index);
    void updateSafetyIndicator();
    void handleExport();
};