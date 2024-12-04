#pragma once
#include "basepage.h"
#include "dataset.h"
#include <QObject>
#include <QMap>
#include <QLineSeries>
#include <QScatterSeries>
#include <QChartView>
#include <QComboBox>
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QVector>

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
class QDateTimeAxis;

// Structure to store processed data points with quality information
struct ProcessedDataPoint {
    QDateTime dateTime;
    double value;
    bool belowDetectionLimit;
    QString pollutantType;
    QString samplingPoint;
    double qualityScore;
};

class POPsPage : public BasePage {
    Q_OBJECT
public:
    explicit POPsPage(QWidget* parent = nullptr);

private:
    void setupUI() override;
    void setupControls();
    void setupDataDisplay();
    void setupInfoPanel();
    void loadData();
    void updateDisplay(int index);
    void updateTimeRange(int index);
    void updateThresholds(const QString& pollutant);
    void updateSafetyIndicator();
    void handleExport();
    double getWarningThreshold(const QString& pollutant);
    double getDangerThreshold(const QString& pollutant);
    double getCurrentLevel();
    bool eventFilter(QObject* obj, QEvent* event) override;
    
    // New data processing methods
    double calculateQualityScore(const ProcessedDataPoint& point, const QDateTime& latestDate);
    void addQualityIndicator(const QPointF& point, double qualityScore);
    void updateChartAxes(const QDateTime& startDate, const QDateTime& endDate, double maxValue);

    // Data handling
    WaterQualityDataset* dataset;
    QVector<ProcessedDataPoint> processedData;

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

    // Constants for known POPs
    const QSet<QString> knownPOPs {
        "Endrin", "DDT", "PCBs", "Dioxins", "Dichlorvos",
        "Chlordane", "Heptachlor", "Toxaphene", "Mirex",
        "Hexachlorobenzene", "Aldrin", "Dieldrin"
    };
};