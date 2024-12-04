#include "popspage.h"
#include "dataset.h"
#include "qualitysample.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>
#include <stdexcept>
#include <QCategoryAxis>
#include <QDateTimeAxis>
#include <QToolTip>
#include <QFileDialog>
#include <QDateTime>

bool POPsPage::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QPointF pos = chartView->mapTo(chartView, helpEvent->pos());
        
        // Find nearest data point
        qreal minDistance = std::numeric_limits<qreal>::max();
        QPointF nearestPoint;
        QString tooltipText;
        
        for (int i = 0; i < currentLevelSeries->count(); ++i) {
            QPointF point = currentLevelSeries->at(i);
            qreal distance = QLineF(pos, point).length();
            
            if (distance < minDistance) {
                minDistance = distance;
                nearestPoint = point;
                
                QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(point.x());
                tooltipText = QString("Date: %1\nLevel: %2 μg/L")
                    .arg(dateTime.toString("yyyy-MM-dd"))
                    .arg(point.y(), 0, 'f', 2);
            }
        }
        
        if (minDistance < 50) {  // Show tooltip only if mouse is close enough
            QToolTip::showText(helpEvent->globalPos(), tooltipText);
        } else {
            QToolTip::hideText();
        }
        
        return true;
    }
    return BasePage::eventFilter(obj, event);
}
double POPsPage::getCurrentLevel() {
    if (currentLevelSeries->count() > 0) {
        return currentLevelSeries->at(currentLevelSeries->count() - 1).y();
    }
    return 0.0;
}

POPsPage::POPsPage(QWidget* parent)
    : BasePage("Persistent Organic Pollutants", parent)
    , dataset(new WaterQualityDataset())
    , chart(nullptr)
    , currentLevelSeries(nullptr)
    , warningThresholdSeries(nullptr)
    , dangerThresholdSeries(nullptr)
    , chartView(nullptr)
    , axisY(nullptr)
{
    // Initialize chart and series before loading data
    chart = new QChart();
    currentLevelSeries = new QLineSeries(this);
    warningThresholdSeries = new QLineSeries(this);
    dangerThresholdSeries = new QLineSeries(this);
    
    setupUI();
    loadData();
    
    // Connect signals
    connect(pollutantSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &POPsPage::updateDisplay);
    connect(timeRangeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &POPsPage::updateTimeRange);
    connect(exportButton, &QPushButton::clicked,
            this, &POPsPage::handleExport);
}

double POPsPage::calculateQualityScore(const ProcessedDataPoint& point, const QDateTime& latestDate) {
    double score = 1.0; // Start with perfect score

    // Reduce score for older data
    int daysOld = point.dateTime.daysTo(latestDate);
    if (daysOld > 365) {
        score *= 0.8; // Older than a year
    } else if (daysOld > 180) {
        score *= 0.9; // Older than 6 months
    } else if (daysOld > 90) {
        score *= 0.95; // Older than 3 months
    }

    // Reduce score for values below detection limit
    if (point.belowDetectionLimit) {
        score *= 0.9;
    }

    // Ensure score stays between 0 and 1
    return qBound(0.0, score, 1.0);
}

void POPsPage::loadData() {
    if (!dataset) {
        qWarning() << "Dataset not initialized";
        return;
    }

    try {
        dataset->loadData("../dataset/Y-2024-M.csv");
        dataset->checkDataExists();
        
        processedData.clear();
        QDateTime latestDate;
        bool firstValidDate = true;
        
        for (size_t i = 0; i < dataset->size(); ++i) {
            const auto& sample = dataset->getData()[i];
            QString pollutant = QString::fromStdString(sample.getDeterminandLabel());
            
            if (!knownPOPs.contains(pollutant)) {
                continue;
            }
            
            QDateTime sampleDate = QDateTime::fromString(
                QString::fromStdString(sample.getSamplingPointDateTime()),
                Qt::ISODate
            );
            
            if (!sampleDate.isValid()) {
                qWarning() << "Invalid date format for sample:" << sample.getId().c_str();
                continue;
            }
            
            if (firstValidDate || sampleDate > latestDate) {
                latestDate = sampleDate;
                firstValidDate = false;
            }
            
            QString resultStr = QString::fromStdString(sample.getResult());
            bool belowDetectionLimit = resultStr.startsWith('<');
            
            if (belowDetectionLimit) {
                resultStr = resultStr.mid(1); // Remove '<' prefix
            }
            
            bool conversionOk;
            double value = resultStr.toDouble(&conversionOk);
            
            if (!conversionOk) {
                qWarning() << "Invalid value format for sample:" << sample.getId().c_str();
                continue;
            }
            
            // For values below detection limit, use half the detection limit
            if (belowDetectionLimit) {
                value = value / 2.0;
            }
            
            ProcessedDataPoint point;
            point.dateTime = sampleDate;
            point.value = value;
            point.belowDetectionLimit = belowDetectionLimit;
            point.pollutantType = pollutant;
            point.samplingPoint = QString::fromStdString(sample.getSamplingPointLabel());
            point.qualityScore = calculateQualityScore(point, latestDate);
            
            processedData.append(point);
        }
        
        if (pollutantSelector) {
            updateDisplay(pollutantSelector->currentIndex());
        }
        
    } catch (const std::exception& e) {
        qWarning() << "Error loading data:" << e.what();
    }
}
void POPsPage::updateChartAxes(const QDateTime& startDate, const QDateTime& endDate, double maxValue) {
    if (!chart) {
        qWarning() << "Chart not initialized";
        return;
    }

    // Remove existing axes
    const auto axes = chart->axes();
    for (auto axis : axes) {
        chart->removeAxis(axis);
    }

    // Create and configure X axis (time)
    QDateTimeAxis* axisX = new QDateTimeAxis(chart);
    
    // Calculate appropriate time format based on date range
    qint64 daysDiff = startDate.daysTo(endDate);
    if (daysDiff <= 31) {
        axisX->setFormat("dd MMM");
        axisX->setTickCount(std::min(daysDiff + 1, 10LL));
    } else if (daysDiff <= 92) {
        axisX->setFormat("MMM dd");
        axisX->setTickCount(8);
    } else {
        axisX->setFormat("MMM yyyy");
        axisX->setTickCount(6);
    }
    
    axisX->setTitleText("Date");
    axisX->setRange(startDate, endDate);
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);

    // Create and configure Y axis
    axisY = new QValueAxis(chart);
    QString selectedPollutant = pollutantSelector->currentText();
    
    // Set Y-axis range based on pollutant type and thresholds
    double minValue = 0.0;
    double yAxisMax;
    
    // Get threshold values
    double warningLevel = getWarningThreshold(selectedPollutant);
    double dangerLevel = getDangerThreshold(selectedPollutant);
    
    // Set maximum Y value to be at least 20% above the danger threshold
    yAxisMax = std::max(dangerLevel * 1.2, maxValue * 1.2);
    
    // Ensure minimum range shows thresholds clearly
    if (selectedPollutant == "PCBs") {
        yAxisMax = std::max(8.0, yAxisMax); // Show up to 8 μg/L minimum for PCBs
    }
    
    axisY->setRange(minValue, yAxisMax);
    axisY->setTickCount(8);
    axisY->setMinorTickCount(1);
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Concentration (μg/L)");
    chart->addAxis(axisY, Qt::AlignLeft);

    // Attach series to axes
    if (currentLevelSeries) {
        currentLevelSeries->attachAxis(axisX);
        currentLevelSeries->attachAxis(axisY);
    }
    if (warningThresholdSeries) {
        warningThresholdSeries->attachAxis(axisX);
        warningThresholdSeries->attachAxis(axisY);
    }
    if (dangerThresholdSeries) {
        dangerThresholdSeries->attachAxis(axisX);
        dangerThresholdSeries->attachAxis(axisY);
    }
}
void POPsPage::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(contentArea);
    
    // Create left panel for data display
    QWidget* dataPanel = new QWidget();
    dataLayout = new QVBoxLayout(dataPanel);
    dataLayout->setSpacing(10);
    
    // Create right panel for info
    infoPanel = new QFrame();
    infoPanel->setObjectName("infoPanel");
    infoPanel->setStyleSheet(R"(
        QFrame#infoPanel {
            border: 1px solid #037a9b;
            border-radius: 8px;
            margin: 5px;
            padding: 15px;
            background-color: #f8f9fa;
        }
    )");
    
    // Add shadow effect to info panel
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(2, 2);
    infoPanel->setGraphicsEffect(shadow);
    
    infoPanelLayout = new QVBoxLayout(infoPanel);
    infoPanel->setFixedWidth(300);
    
    setupControls();
    setupDataDisplay();
    setupInfoPanel();
    
    mainLayout->addWidget(dataPanel, 2);
    mainLayout->addWidget(infoPanel, 1);
}

void POPsPage::setupControls() {
    QFrame* controlsFrame = new QFrame();
    controlsFrame->setObjectName("controlsFrame");
    controlsFrame->setStyleSheet(R"(
        QFrame#controlsFrame {
            background-color: #ffffff;
            border: 1px solid #dee2e6;
            border-radius: 8px;
            padding: 15px;
        }
        QComboBox {
            min-width: 180px;
            padding: 8px;
            border: 1px solid #ced4da;
            border-radius: 4px;
        }
        QPushButton {
            padding: 8px 20px;
            background-color: #037a9b;
            color: white;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #025e77;
        }
    )");
    
    controlsLayout = new QHBoxLayout(controlsFrame);
    controlsLayout->setSpacing(20);
    
    pollutantSelector = new QComboBox();
    pollutantSelector->addItems({"PCBs", "Dioxins", "DDT", "Other POPs"});
    
    timeRangeSelector = new QComboBox();
    timeRangeSelector->addItems({"Last Month", "Last Quarter", "Last Year", "All Time"});
    
    exportButton = new QPushButton("Export Data");
    
    QLabel* pollutantLabel = new QLabel("Pollutant:");
    pollutantLabel->setStyleSheet("font-weight: bold;");
    QLabel* timeRangeLabel = new QLabel("Time Range:");
    timeRangeLabel->setStyleSheet("font-weight: bold;");
    
    controlsLayout->addWidget(pollutantLabel);
    controlsLayout->addWidget(pollutantSelector);
    controlsLayout->addWidget(timeRangeLabel);
    controlsLayout->addWidget(timeRangeSelector);
    controlsLayout->addStretch();
    controlsLayout->addWidget(exportButton);
    
    dataLayout->addWidget(controlsFrame);
}

void POPsPage::setupDataDisplay() {
    dataDisplayFrame = new QFrame();
    dataDisplayFrame->setObjectName("dataDisplay");
    dataDisplayFrame->setStyleSheet(R"(
        QFrame#dataDisplay {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            background-color: white;
            min-height: 500px;
            padding: 20px;
        }
    )");
    
    QVBoxLayout* displayLayout = new QVBoxLayout(dataDisplayFrame);
    
    // Setup chart
    chart = new QChart();
    chart->setTitle("POP Concentration Trends");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    // Create and style series
    currentLevelSeries = new QLineSeries(this);
    currentLevelSeries->setName("Current Levels");
    QPen currentPen(QColor("#037a9b"));
    currentPen.setWidth(3);
    currentLevelSeries->setPen(currentPen);
    
    warningThresholdSeries = new QLineSeries(this);
    warningThresholdSeries->setName("Warning Threshold");
    QPen warningPen(QColor("#ffd700"));
    warningPen.setWidth(2);
    warningPen.setStyle(Qt::DashLine);
    warningThresholdSeries->setPen(warningPen);
    
    dangerThresholdSeries = new QLineSeries(this);
    dangerThresholdSeries->setName("Danger Threshold");
    QPen dangerPen(QColor("#ff4444"));
    dangerPen.setWidth(2);
    dangerPen.setStyle(Qt::DashLine);
    dangerThresholdSeries->setPen(dangerPen);
    
    // Add series to chart
    chart->addSeries(currentLevelSeries);
    chart->addSeries(warningThresholdSeries);
    chart->addSeries(dangerThresholdSeries);
    
    // Create chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(400);
    
    // Create and setup legend frame
    legendFrame = new QFrame();
    legendFrame->setObjectName("legendFrame");
    legendFrame->setStyleSheet(R"(
        QFrame#legendFrame {
            background-color: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 4px;
            padding: 10px;
            margin-top: 10px;
        }
    )");
    
    QHBoxLayout* legendLayout = new QHBoxLayout(legendFrame);
    legendLayout->setSpacing(20);
    
    auto createLegendItem = [](const QString& text, const QString& color, const QString& style = "solid") {
        QWidget* container = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(5);
        
        QFrame* line = new QFrame();
        line->setFixedSize(20, 2);
        QString styleSheet = QString("background-color: %1;").arg(color);
        if (style == "dashed") {
            styleSheet += "border: none; border-top: 2px dashed " + color + ";";
        }
        line->setStyleSheet(styleSheet);
        
        QLabel* label = new QLabel(text);
        
        layout->addWidget(line);
        layout->addWidget(label);
        layout->addStretch();
        
        return container;
    };
    
    legendLayout->addWidget(createLegendItem("Current Levels", "#037a9b"));
    legendLayout->addWidget(createLegendItem("Warning Threshold", "#ffd700", "dashed"));
    legendLayout->addWidget(createLegendItem("Danger Threshold", "#ff4444", "dashed"));
    legendLayout->addStretch();
    
    // Add widgets to layout
    displayLayout->addWidget(chartView);
    displayLayout->addWidget(legendFrame);
    
    if (dataLayout) {
        dataLayout->addWidget(dataDisplayFrame);
    }
    
    // Install event filter for tooltips
    chartView->installEventFilter(this);
}
void POPsPage::setupInfoPanel() {
    infoPanelLayout->setSpacing(15);
    
    // Safety Status Section
    QFrame* safetyFrame = new QFrame();
    safetyFrame->setObjectName("safetyFrame");
    safetyFrame->setStyleSheet(R"(
        QFrame#safetyFrame {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            padding: 15px;
            background-color: white;
        }
        QLabel#statusLabel {
            font-weight: bold;
            padding: 10px;
            border-radius: 6px;
            color: white;
            background-color: #28a745;
        }
    )");
    
    QVBoxLayout* safetyLayout = new QVBoxLayout(safetyFrame);
    QLabel* safetyTitle = new QLabel("Current Safety Status");
    safetyTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    safetyLevelIndicator = new QLabel("Safe - Within Limits");
    safetyLevelIndicator->setObjectName("statusLabel");
    safetyLevelIndicator->setAlignment(Qt::AlignCenter);
    
    safetyLayout->addWidget(safetyTitle);
    safetyLayout->addWidget(safetyLevelIndicator);
    
    // Health Risks Section with hover effect
    QFrame* healthFrame = new QFrame();
    healthFrame->setObjectName("healthFrame");
    healthFrame->setStyleSheet(R"(
        QFrame#healthFrame {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            padding: 15px;
            background-color: white;
        }
        QFrame#healthFrame:hover {
            background-color: #f8f9fa;
        }
    )");
    
    QVBoxLayout* healthLayout = new QVBoxLayout(healthFrame);
    QLabel* healthTitle = new QLabel("Health Risks");
    healthTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    healthRiskLabel = new QLabel("Long-term exposure to PCBs may cause:\n• Liver damage\n• Immune system effects\n• Cancer risks\n• Developmental issues");
    healthRiskLabel->setWordWrap(true);
    
    healthLayout->addWidget(healthTitle);
    healthLayout->addWidget(healthRiskLabel);
    
    // Threshold Information
    QFrame* thresholdFrame = new QFrame();
    thresholdFrame->setObjectName("thresholdFrame");
    thresholdFrame->setStyleSheet(R"(
        QFrame#thresholdFrame {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            padding: 15px;
            background-color: white;
        }
    )");
    
    QVBoxLayout* thresholdLayout = new QVBoxLayout(thresholdFrame);
    QLabel* thresholdTitle = new QLabel("Safety Thresholds");
    thresholdTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    thresholdLabel = new QLabel(
        "UK/EU Safety Thresholds for PCBs:\n"
        "• Safe: < 5 μg/L\n"
        "• Warning: 5-7 μg/L\n"
        "• Danger: > 7 μg/L"
    );
    thresholdLabel->setWordWrap(true);
    
    thresholdLayout->addWidget(thresholdTitle);
    thresholdLayout->addWidget(thresholdLabel);
    
    // Add all sections to info panel
    infoPanelLayout->addWidget(safetyFrame);
    infoPanelLayout->addWidget(healthFrame);
    infoPanelLayout->addWidget(thresholdFrame);
    infoPanelLayout->addStretch();
    
    // Connect signals
    connect(pollutantSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &POPsPage::updateDisplay);
    connect(timeRangeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &POPsPage::updateTimeRange);
    connect(exportButton, &QPushButton::clicked,
            this, &POPsPage::handleExport);
}

double POPsPage::getDangerThreshold(const QString& pollutant) {
    // Define danger thresholds for different pollutants
    static const QMap<QString, double> thresholds = {
        {"PCBs", 7.0},
        {"Dioxins", 0.2},
        {"DDT", 2.0},
        {"Other POPs", 4.0}
    };
    return thresholds.value(pollutant, 7.0);
}

void POPsPage::updateThresholds(const QString& pollutant) {
    double warningLevel = getWarningThreshold(pollutant);
    double dangerLevel = getDangerThreshold(pollutant);
    
    // Clear existing threshold lines
    warningThresholdSeries->clear();
    dangerThresholdSeries->clear();
    
    // Get current time range
    QString timeRange = timeRangeSelector->currentText();
    QDateTime endDate = QDateTime::currentDateTime();
    QDateTime startDate;
    
    if (timeRange == "Last Month") {
        startDate = endDate.addMonths(-1);
    } else if (timeRange == "Last Quarter") {
        startDate = endDate.addMonths(-3);
    } else if (timeRange == "Last Year") {
        startDate = endDate.addYears(-1);
    } else {
        startDate = endDate.addYears(-5);
    }

    // Add points at regular intervals for smoother lines
    qint64 interval = startDate.daysTo(endDate) / 10; // Create 10 segments
    for (int i = 0; i <= 10; i++) {
        QDateTime pointDate = startDate.addDays(i * interval);
        qint64 msecsSinceEpoch = pointDate.toMSecsSinceEpoch();
        
        warningThresholdSeries->append(msecsSinceEpoch, warningLevel);
        dangerThresholdSeries->append(msecsSinceEpoch, dangerLevel);
    }
}
double POPsPage::getWarningThreshold(const QString& pollutant) {
    // Define warning thresholds for different pollutants
    static const QMap<QString, double> thresholds = {
        {"PCBs", 5.0},
        {"Dioxins", 0.1},
        {"DDT", 1.0},
        {"Other POPs", 2.0}
    };
    return thresholds.value(pollutant, 5.0);
}
void POPsPage::addQualityIndicator(const QPointF& point, double qualityScore) {
    if (!chart) {
        qWarning() << "Chart not initialized";
        return;
    }

    QScatterSeries* indicator = new QScatterSeries(this);  // Add parent
    if (!indicator) {
        qWarning() << "Failed to create scatter series";
        return;
    }

    indicator->append(point);
    indicator->setMarkerSize(8);
    indicator->setColor(QColor::fromHsv(
        int(120 * qualityScore), // Hue (120 for green, reduces towards red)
        200, // Saturation
        200  // Value
    ));
    indicator->setBorderColor(Qt::transparent);
    
    chart->addSeries(indicator);
    
    if (chart->axes().size() >= 2) {
        indicator->attachAxis(chart->axes().at(0));
        indicator->attachAxis(chart->axes().at(1));
    }
}


void POPsPage::updateDisplay(int index) {
    QString selectedPollutant = pollutantSelector->currentText();
    QString timeRange = timeRangeSelector->currentText();
    
    // Clear existing series data
    currentLevelSeries->clear();
    
    QDateTime endDate = QDateTime::currentDateTime();
    QDateTime startDate;
    
    // Set time range
    if (timeRange == "Last Month") {
        startDate = endDate.addMonths(-1);
    } else if (timeRange == "Last Quarter") {
        startDate = endDate.addMonths(-3);
    } else if (timeRange == "Last Year") {
        startDate = endDate.addYears(-1);
    } else {
        startDate = endDate.addYears(-5);
    }
    
    // Filter and add data points
    double maxValue = 0;
    for (const auto& point : processedData) {
        if (point.dateTime >= startDate && 
            point.dateTime <= endDate &&
            point.pollutantType == selectedPollutant) {
            
            currentLevelSeries->append(point.dateTime.toMSecsSinceEpoch(), point.value);
            maxValue = qMax(maxValue, point.value);
            
            // Add quality indicator
            if (point.qualityScore < 0.9) {
                // Add visual indicator for lower quality data
                QPointF dataPoint(point.dateTime.toMSecsSinceEpoch(), point.value);
                addQualityIndicator(dataPoint, point.qualityScore);
            }
        }
    }
    
    // Update axes and other chart elements
    updateTimeRange(timeRangeSelector->currentIndex());
    updateChartAxes(startDate, endDate, maxValue);
    updateThresholds(selectedPollutant);
    updateSafetyIndicator();
}

void POPsPage::updateTimeRange(int index) {
    // Get currently selected pollutant and time range
    QString selectedPollutant = pollutantSelector->currentText();
    QString timeRange = timeRangeSelector->currentText();

    // Clear existing data
    currentLevelSeries->clear();
    
    // Set time range based on selection
    QDateTime endDate = QDateTime::currentDateTime();
    QDateTime startDate;
    
    // Calculate start date based on selected range
    if (timeRange == "Last Month") {
        startDate = endDate.addMonths(-1);
    } else if (timeRange == "Last Quarter") {
        startDate = endDate.addMonths(-3);
    } else if (timeRange == "Last Year") {
        startDate = endDate.addYears(-1);
    } else { // All Time
        startDate = endDate.addYears(-5); // Default to 5 years of historical data
    }

    // Filter and add data points
    double maxValue = 0;
    int pointCount = 0;
    
    for (const auto& point : processedData) {
        if (point.dateTime >= startDate && 
            point.dateTime <= endDate &&
            point.pollutantType == selectedPollutant) {
            
            currentLevelSeries->append(point.dateTime.toMSecsSinceEpoch(), point.value);
            maxValue = qMax(maxValue, point.value);
            pointCount++;
            
            // Add quality indicator for lower quality data
            if (point.qualityScore < 0.9) {
                QPointF dataPoint(point.dateTime.toMSecsSinceEpoch(), point.value);
                addQualityIndicator(dataPoint, point.qualityScore);
            }
        }
    }

    // Update axes with new time range and data values
    updateChartAxes(startDate, endDate, maxValue);
    
    // Update thresholds for the new time range
    updateThresholds(selectedPollutant);
    
    // If no data points found, show a message or handle empty state
    if (pointCount == 0) {
        // Add a default range if no data points exist
        if (axisY) {
            axisY->setRange(0, getWarningThreshold(selectedPollutant) * 2);
        }
    }

    // Update safety indicator based on latest data
    updateSafetyIndicator();

    // Update chart title to include time range
    QString titleRange;
    if (timeRange == "Last Month") {
        titleRange = "Past Month";
    } else if (timeRange == "Last Quarter") {
        titleRange = "Past 3 Months";
    } else if (timeRange == "Last Year") {
        titleRange = "Past Year";
    } else {
        titleRange = "All Time";
    }
    
    chart->setTitle(QString("%1 Concentration Trends (%2)")
        .arg(selectedPollutant)
        .arg(titleRange));
}

void POPsPage::updateSafetyIndicator() {
    double currentLevel = getCurrentLevel();
    double warningLevel = getWarningThreshold(pollutantSelector->currentText());
    double dangerLevel = getDangerThreshold(pollutantSelector->currentText());
    
    QString status;
    QString color;
    
    if (currentLevel < warningLevel) {
        status = "Safe - Within Limits";
        color = "#28a745";  // Green
    } else if (currentLevel < dangerLevel) {
        status = "Warning - Elevated Levels";
        color = "#ffc107";  // Yellow
    } else {
        status = "Danger - Exceeds Limits";
        color = "#dc3545";  // Red
    }
    
    safetyLevelIndicator->setText(status);
    safetyLevelIndicator->setStyleSheet(QString("background-color: %1; color: white; padding: 10px; border-radius: 6px;").arg(color));
}

void POPsPage::handleExport() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export Data", "", "CSV Files (*.csv);;All Files (*)");
        
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    
    QTextStream stream(&file);
    // Write headers matching the original format
    stream << "@id,sample.samplingPoint.notation,sample.samplingPoint.label,"
           << "sample.getSamplingPointDateTime,determinand.label,determinand.definition,"
           << "determinand.notation,result,determinand.unit.label,"
           << "sample.sampledMaterialType.label,sample.purpose.label,"
           << "sample.samplingPoint.easting,sample.samplingPoint.northing\n";
    
    QString selectedPollutant = pollutantSelector->currentText();
    
 
    for (const auto& sample : dataset->getData()) {
        if (QString::fromStdString(sample.getDeterminandLabel()) == selectedPollutant) {
            stream << QString::fromStdString(sample.getId()) << ","
                << QString::fromStdString(sample.getSamplingPointNotation()) << ","
                << QString::fromStdString(sample.getSamplingPointLabel()) << ","
                << QString::fromStdString(sample.getSamplingPointDateTime()) << ","
                << QString::fromStdString(sample.getDeterminandLabel()) << ","
                << QString::fromStdString(sample.getDeterminandDefinition()) << ","
                << QString::fromStdString(sample.getDeterminandNotation()) << ","
                << QString::fromStdString(sample.getResult()) << ","
                << QString::fromStdString(sample.getDeterminandUnitLabel()) << ","
                << QString::fromStdString(sample.getSampledMaterialType()) << ","
                << QString::fromStdString(sample.getPurposeLabel()) << ","
                << QString::fromStdString(sample.getEasting()) << ","
                << QString::fromStdString(sample.getNorthing()) << "\n";
        }
    }
    
    file.close();
}