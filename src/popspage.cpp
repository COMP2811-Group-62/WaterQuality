 #include "popspage.h"
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

POPsPage::POPsPage(QWidget* parent)
    : BasePage("Persistent Organic Pollutants", parent) {
    setupUI();
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
        QFrame#graphArea {
            border: 1px solid #e9ecef;
            border-radius: 8px;
            background-color: #ffffff;
            margin: 10px;
        }
        QFrame#legendFrame {
            background-color: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 4px;
            padding: 15px;
            margin-top: 15px;
        }
    )");
    
    QVBoxLayout* displayLayout = new QVBoxLayout(dataDisplayFrame);
    
    // Create and setup chart
    chart = new QChart();
    chart->setTitle("POP Concentration Trends");
    chart->setAnimationOptions(QChart::AllAnimations);
    
    // Create series for current levels and thresholds
    currentLevelSeries = new QLineSeries();
    warningThresholdSeries = new QLineSeries();
    dangerThresholdSeries = new QLineSeries();
    
    for (int i = 0; i < 6; ++i) {
    currentLevelSeries->append(i, QRandomGenerator::global()->bounded(10));
    warningThresholdSeries->append(i, 5);
    dangerThresholdSeries->append(i, 7);
    }
    
    // Style the series
    QPen currentPen(QColor("#037a9b"));
    currentPen.setWidth(3);
    currentLevelSeries->setPen(currentPen);
    
    QPen warningPen(QColor("#ffd700"));
    warningPen.setWidth(2);
    warningPen.setStyle(Qt::DashLine);
    warningThresholdSeries->setPen(warningPen);
    
    QPen dangerPen(QColor("#ff4444"));
    dangerPen.setWidth(2);
    dangerPen.setStyle(Qt::DashLine);
    dangerThresholdSeries->setPen(dangerPen);
    
    chart->addSeries(currentLevelSeries);
    chart->addSeries(warningThresholdSeries);
    chart->addSeries(dangerThresholdSeries);
    
    // Create chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Create legend
    legendFrame = new QFrame();
    legendFrame->setObjectName("legendFrame");
    QHBoxLayout* legendLayout = new QHBoxLayout(legendFrame);
    
    auto createLegendItem = [](const QString& text, const QString& color) {
        QFrame* container = new QFrame();
        QHBoxLayout* layout = new QHBoxLayout(container);
        QFrame* colorBox = new QFrame();
        colorBox->setFixedSize(16, 16);
        colorBox->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color));
        QLabel* label = new QLabel(text);
        layout->addWidget(colorBox);
        layout->addWidget(label);
        return container;
    };
    
    legendLayout->addWidget(createLegendItem("Current Levels", "#037a9b"));
    legendLayout->addWidget(createLegendItem("Warning Threshold", "#ffd700"));
    legendLayout->addWidget(createLegendItem("Danger Threshold", "#ff4444"));
    legendLayout->addStretch();
    
    displayLayout->addWidget(chartView);
    displayLayout->addWidget(legendFrame);
    
    dataLayout->addWidget(dataDisplayFrame);
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

void POPsPage::updateDisplay(int index) {
    // Placeholder for chart updating
    updateSafetyIndicator();
}

void POPsPage::updateTimeRange(int index) {
    // Placeholder for time range updating
}

void POPsPage::updateSafetyIndicator() {
    // Placeholder for safety indicator updating
}

void POPsPage::handleExport() {
    // Placeholder for data export functionality
}