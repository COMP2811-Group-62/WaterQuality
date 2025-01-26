#include "popspage.h"

#include <QCategoryAxis>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QFileDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineSeries>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QToolTip>
#include <QVBoxLayout>
#include <stdexcept>

#include "dataset.h"
#include "qualitysample.h"
#include "styles.h"

POPsPage::POPsPage(SampleModel* model, QWidget* parent)
    : BasePage(tr("Persistent Organic Pollutants"), parent), model(model) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/popspage.qss"}));
  setupUI();
  loadData();

  // Connect signals - update both when pollutant changes
  connect(pollutantSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) {
            updateTimeRangeOptions(pollutantSelector->currentText());
            updateDisplay(index);
            updateTimeRange(timeRangeSelector->currentIndex());
            updateLocationSelector(pollutantSelector->currentText());
          });
  connect(timeRangeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &POPsPage::updateTimeRange);
  connect(exportButton, &QPushButton::clicked,
          this, &POPsPage::handleExport);

  // Set initial selection to Full Year
  int fullYearIndex = timeRangeSelector->findText(tr("Full Year 2024"));
  if (fullYearIndex != -1) {
    timeRangeSelector->setCurrentIndex(fullYearIndex);
  }
}

void POPsPage::refreshView() {
  loadData();
  updateDisplay(pollutantSelector->currentIndex());
  updateTimeRange(timeRangeSelector->currentIndex());
}

void POPsPage::setupUI() {
  QHBoxLayout* mainLayout = new QHBoxLayout(contentArea);

  // Create left panel for data display
  QWidget* dataPanel = new QWidget();
  dataLayout = new QVBoxLayout(dataPanel);

  // Create right panel for info
  infoPanel = new QFrame();
  infoPanel->setObjectName("infoPanel");
  infoPanelLayout = new QVBoxLayout(infoPanel);

  setupControls();
  setupDataDisplay();
  setupInfoPanel();

  mainLayout->addWidget(dataPanel);
  mainLayout->addWidget(infoPanel);
}

void POPsPage::setupControls() {
  QFrame* controlsFrame = new QFrame();
  controlsFrame->setObjectName("controlsFrame");

  controlsLayout = new QHBoxLayout(controlsFrame);

  QVBoxLayout* pollutantVLabel = new QVBoxLayout();
  QLabel* pollutantLabel = new QLabel(tr("Pollutant:"));
  pollutantSelector = new QComboBox();
  pollutantSelector->addItems({"PCBs", "Endrin",
                               "Aldrin", "Dieldrin"});
  pollutantVLabel->addWidget(pollutantLabel);
  pollutantVLabel->addWidget(pollutantSelector);

  QVBoxLayout* timeRangeVLabel = new QVBoxLayout();
  QLabel* timeRangeLabel = new QLabel(tr("Time Range:"));
  timeRangeSelector = new QComboBox();
  timeRangeSelector->addItems({tr("January 2024"), tr("February 2024"), tr("March 2024"),
                               tr("April 2024"), tr("May 2024"), tr("June 2024"),
                               tr("July 2024"), tr("August 2024"), tr("September 2024"),
                               tr("Full Year 2024")});
  exportButton = new QPushButton(tr("Export Data"));
  timeRangeVLabel->addWidget(timeRangeLabel);
  timeRangeVLabel->addWidget(timeRangeSelector);

  controlsLayout->addLayout(pollutantVLabel);
  controlsLayout->addLayout(timeRangeVLabel);

  setupLocationSelector();  // Add location selector

  controlsLayout->addStretch();
  controlsLayout->addWidget(exportButton);

  dataLayout->addWidget(controlsFrame);
}

void POPsPage::setupDataDisplay() {
  dataDisplayFrame = new QFrame();
  dataDisplayFrame->setObjectName("dataDisplay");

  QVBoxLayout* displayLayout = new QVBoxLayout(dataDisplayFrame);

  // Setup chart
  chart = new QChart();
  chart->setTitle(tr("POP Concentration Trends"));
  chart->setAnimationOptions(QChart::AllAnimations);
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  // Create and style series with smoother lines
  currentLevelSeries = new QLineSeries(this);
  currentLevelSeries->setName(tr("Current Levels"));
  QPen currentPen(QColor("#037a9b"));
  currentPen.setWidth(2);                  // Reduced from 3 for cleaner appearance
  currentPen.setJoinStyle(Qt::RoundJoin);  // Add rounded joins for smoother curves
  currentLevelSeries->setPen(currentPen);

  warningThresholdSeries = new QLineSeries(this);
  warningThresholdSeries->setName(tr("Warning Threshold"));
  QPen warningPen(QColor("#ffd700"));
  warningPen.setWidth(2);
  warningPen.setStyle(Qt::DashLine);
  warningThresholdSeries->setPen(warningPen);

  dangerThresholdSeries = new QLineSeries(this);
  dangerThresholdSeries->setName(tr("Danger Threshold"));
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
  chartView->setBackgroundBrush(Qt::transparent);
  chartView->setStyleSheet("background: transparent;");
  // Install event filter for tooltips
  chartView->setMouseTracking(true);
  chartView->installEventFilter(this);

  // Add widgets to layout
  displayLayout->addWidget(chartView);

  if (dataLayout) {
    dataLayout->addWidget(dataDisplayFrame);
  }

  // Install event filter for tooltips
  chartView->installEventFilter(this);
}

void POPsPage::setupInfoPanel() {
  // Safety Status Section
  QFrame* safetyFrame = new QFrame();
  safetyFrame->setObjectName("safetyFrame");

  QVBoxLayout* safetyLayout = new QVBoxLayout(safetyFrame);
  QLabel* safetyTitle = new QLabel(tr("Current Safety Status"));
  safetyTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
  safetyLevelIndicator = new QLabel(tr("Safe - Within Limits"));
  safetyLevelIndicator->setObjectName("statusLabel");
  safetyLevelIndicator->setAlignment(Qt::AlignCenter);

  safetyLayout->addWidget(safetyTitle);
  safetyLayout->addWidget(safetyLevelIndicator);

  // Health Risks Section with hover effect
  QFrame* healthFrame = new QFrame();
  healthFrame->setObjectName("healthFrame");

  QVBoxLayout* healthLayout = new QVBoxLayout(healthFrame);
  QLabel* healthTitle = new QLabel(tr("Health Risks"));
  healthTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
  healthRiskLabel = new QLabel(tr("Long-term exposure to PCBs may cause:\n• Liver damage\n• Immune system effects\n• Cancer risks\n• Developmental issues"));
  healthRiskLabel->setWordWrap(true);

  healthLayout->addWidget(healthTitle);
  healthLayout->addWidget(healthRiskLabel);

  // Threshold Information
  QFrame* thresholdFrame = new QFrame();
  thresholdFrame->setObjectName("thresholdFrame");

  QVBoxLayout* thresholdLayout = new QVBoxLayout(thresholdFrame);
  QLabel* thresholdTitle = new QLabel(tr("Safety Thresholds"));
  thresholdTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
  thresholdLabel = new QLabel(tr(
      "UK/EU Safety Thresholds for PCBs:\n"
      "• Safe: < 0.005 μg/L\n"
      "• Warning: 0.005-0.007 μg/L\n"
      "• Danger: > 0.007 μg/L"));
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

void POPsPage::loadData() {
  if (!model->hasData()) {
    qWarning() << "Dataset not initialized";
    return;
  }

  processedData.clear();
  availableLocations.clear();
  QDateTime latestDate;
  bool firstValidDate = true;

  for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {
    QString pollutant = model->data(model->index(row, 4), Qt::DisplayRole).toString();

    // Add debug print to see what pollutants are being found
    // qDebug() << "Found pollutant:" << pollutant;

    // Include all PCB-related measurements
    bool isPCB = pollutant.contains("PCB", Qt::CaseInsensitive);
    if (isPCB) {
      pollutant = "PCBs";  // Normalize to the category name
    }

    if (!isPCB && !knownPOPs.contains(pollutant)) {
      continue;
    }

    QString dateStr = model->data(model->index(row, 3), Qt::DisplayRole).toString();
    QDateTime sampleDate = QDateTime::fromString(dateStr, Qt::ISODate);

    if (!sampleDate.isValid()) {
      qWarning() << "Invalid date format for sample at row:" << row;
      continue;
    }

    QString resultStr = model->data(model->index(row, 7), Qt::DisplayRole).toString();
    bool belowDetectionLimit = resultStr.startsWith('<');

    if (belowDetectionLimit) {
      resultStr = resultStr.mid(1);
    }

    bool conversionOk;
    double value = resultStr.toDouble(&conversionOk);

    if (!conversionOk) {
      qWarning() << "Invalid value format for sample at row:" << row;
      continue;
    }

    QString location = model->data(model->index(row, 2), Qt::DisplayRole).toString();
    availableLocations.insert(location);

    ProcessedDataPoint point;
    point.dateTime = sampleDate;
    point.value = value;
    point.belowDetectionLimit = belowDetectionLimit;
    point.pollutantType = pollutant;
    point.samplingPoint = location;
    point.qualityScore = calculateQualityScore(point, latestDate);

    processedData.append(point);
  }

  if (pollutantSelector) {
    updateLocationSelector(pollutantSelector->currentText());
    updateDisplay(pollutantSelector->currentIndex());
  }
}

double POPsPage::calculateQualityScore(const ProcessedDataPoint& point, const QDateTime& latestDate) {
  double score = 1.0;  // Start with perfect score

  // Reduce score for older data
  int daysOld = point.dateTime.daysTo(latestDate);
  if (daysOld > 365) {
    score *= 0.8;  // Older than a year
  } else if (daysOld > 180) {
    score *= 0.9;  // Older than 6 months
  } else if (daysOld > 90) {
    score *= 0.95;  // Older than 3 months
  }

  // Reduce score for values below detection limit
  if (point.belowDetectionLimit) {
    score *= 0.9;
  }

  // Ensure score stays between 0 and 1
  return qBound(0.0, score, 1.0);
}

double POPsPage::getCurrentLevel() {
  if (currentLevelSeries->count() > 0) {
    return currentLevelSeries->at(currentLevelSeries->count() - 1).y();
  }
  return 0.0;
}

void POPsPage::updateDisplay(int index) {
  QString selectedCategory = pollutantSelector->currentText();

  // Define pollutant groups
  QStringList pollutants;
  if (selectedCategory == "Nutrients") {
    pollutants = {
        "Ammonia(N)",
        "NH3 un-ion",
        "Nitrate-N",
        "Nitrite-N",
        "N Oxidised",
        "Nitrogen - N",
        "Phosphorus-P",
        "Orthophospht"};
  } else if (selectedCategory == "Physical Parameters") {
    pollutants = {
        "Temp Water",
        "pH",
        "Cond @ 25C",
        "Alky pH 4.5"};
  } else if (selectedCategory == "Oxygen Related") {
    pollutants = {
        "Oxygen Diss",
        "O Diss %sat"};
  } else if (selectedCategory == "Minerals & Solids") {
    pollutants = {
        "Chloride Ion",
        "Sld Sus@105C",
        "SiO2 Rv"};
  } else {  // Other Parameters
    pollutants = {
        "C - Org Filt",
        "Flow Type"};
  }

  // Clear existing series data
  currentLevelSeries->clear();

  QDateTime endDate = QDateTime::currentDateTime();
  QDateTime startDate;

  // Set time range based on selected month from timeRangeSelector
  QString timeRange = timeRangeSelector->currentText();

  // Filter and sort data points for all pollutants in the selected category
  QVector<QPair<qint64, double>> sortedPoints;
  double maxValue = 0;

  for (const auto& point : processedData) {
    if (point.dateTime >= startDate &&
        point.dateTime <= endDate &&
        pollutants.contains(point.pollutantType)) {
      sortedPoints.append({point.dateTime.toMSecsSinceEpoch(), point.value});
      maxValue = qMax(maxValue, point.value);
    }
  }

  // Sort points by time
  std::sort(sortedPoints.begin(), sortedPoints.end());

  // Add sorted points to series
  for (const auto& point : sortedPoints) {
    currentLevelSeries->append(point.first, point.second);
  }

  // Update thresholds based on category
  updateThresholds(selectedCategory);

  // Update chart title
  chart->setTitle(QString(tr("%1 Trends (%2)"))
                      .arg(selectedCategory)
                      .arg(timeRange));

  // Update y-axis label based on category
  if (axisY) {
    QString unit;
    if (selectedCategory == "Physical Parameters") {
      unit = tr("Various Units");  // Since this category has mixed units
    } else if (selectedCategory == "Oxygen Related") {
      unit = tr("μg/L");
    } else {
      unit = tr("μg/L");  // Default unit
    }
    axisY->setTitleText(QString(tr("Concentration (%1)")).arg(unit));
  }
}

void POPsPage::updateTimeRange(int index) {
  QString selectedPollutant = pollutantSelector->currentText();
  QString timeRange = timeRangeSelector->currentText();

  // Clear existing data
  currentLevelSeries->clear();

  // Set time range based on selection
  QDateTime startDate, endDate;

  // Calculate date range based on selected month
  if (timeRange == tr("Full Year 2024")) {
    startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString("2024-09-30 23:59:59", "yyyy-MM-dd HH:mm:ss");
  } else {
    // Extract month from selection
    QMap<QString, int> monthMap = {
        {tr("January 2024"), 1},
        {tr("February 2024"), 2},
        {tr("March 2024"), 3},
        {tr("April 2024"), 4},
        {tr("May 2024"), 5},
        {tr("June 2024"), 6},
        {tr("July 2024"), 7},
        {tr("August 2024"), 8},
        {tr("September 2024"), 9}};

    int month = monthMap[timeRange];
    int lastDay = QDate(2024, month, 1).daysInMonth();

    startDate = QDateTime::fromString(
        QString("2024-%1-01 00:00:00").arg(month, 2, 10, QChar('0')),
        "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString(
        QString("2024-%1-%2 23:59:59").arg(month, 2, 10, QChar('0')).arg(lastDay),
        "yyyy-MM-dd HH:mm:ss");
  }

  // Filter and sort data points
  QVector<QPair<qint64, double>> sortedPoints;
  double maxValue = 0;

  for (const auto& point : processedData) {
    if (point.dateTime >= startDate &&
        point.dateTime <= endDate &&
        point.pollutantType == selectedPollutant) {
      sortedPoints.append({point.dateTime.toMSecsSinceEpoch(), point.value});
      maxValue = qMax(maxValue, point.value);
    }
  }

  // Sort points by time
  std::sort(sortedPoints.begin(), sortedPoints.end());

  // Add sorted points to series
  for (const auto& point : sortedPoints) {
    currentLevelSeries->append(point.first, point.second);
  }

  // Update axes and other chart elements
  updateChartAxes(startDate, endDate, maxValue);
  updateThresholds(selectedPollutant);
  updateSafetyIndicator();

  // Update chart title
  chart->setTitle(QString(tr("%1 Concentration Trends (%2)"))
                      .arg(selectedPollutant)
                      .arg(timeRange));
}

void POPsPage::updateTimeRangeOptions(const QString& selectedPollutant) {
  // Temporarily block signals to prevent unwanted updates
  timeRangeSelector->blockSignals(true);

  // Store current selection if any
  QString currentSelection = timeRangeSelector->currentText();

  // Clear current items
  timeRangeSelector->clear();

  // Create a set of months where data exists for this pollutant and selected location
  QSet<int> monthsWithData;
  QString selectedLocation = locationSelector->currentText();

  // Check if there's any data at all for this pollutant
  bool hasAnyData = false;

  // Scan through data to find months with data for selected pollutant and location
  for (const auto& point : processedData) {
    if (point.pollutantType == selectedPollutant &&
        (selectedLocation == tr("All Locations") || point.samplingPoint == selectedLocation)) {
      monthsWithData.insert(point.dateTime.date().month());
      hasAnyData = true;
    }
  }

  // Add full year option - grey it out if no data exists
  QStandardItem* fullYearItem = new QStandardItem(tr("Full Year 2024"));
  if (!hasAnyData) {
    fullYearItem->setFlags(fullYearItem->flags() & ~Qt::ItemIsEnabled);
    fullYearItem->setData(QColor(128, 128, 128), Qt::ForegroundRole);
  }
  qobject_cast<QStandardItemModel*>(timeRangeSelector->model())->appendRow(fullYearItem);

  // Add months with custom formatting based on data availability
  QStringList months = {
      tr("January 2024"), tr("February 2024"), tr("March 2024"),
      tr("April 2024"), tr("May 2024"), tr("June 2024"),
      tr("July 2024"), tr("August 2024"), tr("September 2024")};

  for (int i = 0; i < months.size(); ++i) {
    QStandardItem* item = new QStandardItem(months[i]);
    if (!monthsWithData.contains(i + 1)) {
      // Grey out months without data
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      item->setData(QColor(128, 128, 128), Qt::ForegroundRole);
    }
    qobject_cast<QStandardItemModel*>(timeRangeSelector->model())->appendRow(item);
  }

  // Try to restore previous selection if it was valid
  int index = timeRangeSelector->findText(currentSelection);
  if (index != -1 && timeRangeSelector->model()->flags(timeRangeSelector->model()->index(index, 0)) & Qt::ItemIsEnabled) {
    timeRangeSelector->setCurrentIndex(index);
  } else {
    // Select the first enabled item
    for (int i = 0; i < timeRangeSelector->count(); ++i) {
      if (timeRangeSelector->model()->flags(timeRangeSelector->model()->index(i, 0)) & Qt::ItemIsEnabled) {
        timeRangeSelector->setCurrentIndex(i);
        break;
      }
    }
  }

  // Re-enable signals
  timeRangeSelector->blockSignals(false);
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

  axisX->setTitleText(tr("Date"));
  axisX->setRange(startDate, endDate);
  axisX->setLabelsAngle(-45);
  chart->addAxis(axisX, Qt::AlignBottom);

  // Create and configure Y axis
  axisY = new QValueAxis(chart);
  QString selectedCategory = pollutantSelector->currentText();

  // Set Y-axis range based on pollutant category
  double minValue = 0.0;
  double yAxisMax = std::max(maxValue * 1.2, 0.012);  // Set minimum max to 0.012 or 120% of max value

  // Use 3 decimal places for precision
  axisY->setLabelFormat("%.3f");
  axisY->setRange(minValue, yAxisMax);
  axisY->setTickCount(8);
  axisY->setMinorTickCount(1);

  // Set appropriate y-axis label based on category
  if (selectedCategory == "Physical Parameters") {
    axisY->setTitleText(tr("Value (Various Units)"));
  } else {
    axisY->setTitleText(tr("Concentration (μg/L)"));
  }

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

void POPsPage::updateThresholds(const QString& pollutant) {
  double warningLevel = getWarningThreshold(pollutant);
  double dangerLevel = getDangerThreshold(pollutant);

  // Clear existing threshold lines
  warningThresholdSeries->clear();
  dangerThresholdSeries->clear();

  // Get current time range
  QString timeRange = timeRangeSelector->currentText();
  QDateTime startDate, endDate;

  // Calculate date range based on selected month
  if (timeRange == tr("Full Year 2024")) {
    startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString("2024-09-30 23:59:59", "yyyy-MM-dd HH:mm:ss");
  } else {
    QMap<QString, int> monthMap = {
        {tr("January 2024"), 1},
        {tr("February 2024"), 2},
        {tr("March 2024"), 3},
        {tr("April 2024"), 4},
        {tr("May 2024"), 5},
        {tr("June 2024"), 6},
        {tr("July 2024"), 7},
        {tr("August 2024"), 8},
        {tr("September 2024"), 9}};

    int month = monthMap[timeRange];
    int lastDay = QDate(2024, month, 1).daysInMonth();

    startDate = QDateTime::fromString(
        QString("2024-%1-01 00:00:00").arg(month, 2, 10, QChar('0')),
        "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString(
        QString("2024-%1-%2 23:59:59").arg(month, 2, 10, QChar('0')).arg(lastDay),
        "yyyy-MM-dd HH:mm:ss");
  }

  // Add threshold lines
  warningThresholdSeries->append(startDate.toMSecsSinceEpoch(), warningLevel);
  warningThresholdSeries->append(endDate.toMSecsSinceEpoch(), warningLevel);

  dangerThresholdSeries->append(startDate.toMSecsSinceEpoch(), dangerLevel);
  dangerThresholdSeries->append(endDate.toMSecsSinceEpoch(), dangerLevel);
}

void POPsPage::updateSafetyIndicator() {
  double currentLevel = getCurrentLevel();
  double warningLevel = getWarningThreshold(pollutantSelector->currentText());
  double dangerLevel = getDangerThreshold(pollutantSelector->currentText());

  QString status;
  QString color;

  if (currentLevel < warningLevel) {
    status = tr("Safe - Within Limits");
    color = "#28a745";  // Green
  } else if (currentLevel < dangerLevel) {
    status = tr("Warning - Elevated Levels");
    color = "#ffc107";  // Yellow
  } else {
    status = tr("Danger - Exceeds Limits");
    color = "#dc3545";  // Red
  }

  safetyLevelIndicator->setText(status);
  safetyLevelIndicator->setStyleSheet(QString("background-color: %1; color: white; padding: 10px; border-radius: 6px;").arg(color));
}

double POPsPage::getDangerThreshold(const QString& pollutant) {
  static const QMap<QString, double> thresholds = {
      {"PCBs", 0.007},
      {"Dioxins", 0.007},
      {"DDT", 0.007},
      {"Endrin", 0.007},
      {"Aldrin", 0.007},
      {"Dieldrin", 0.007},
      {"Other POPs", 0.007}};
  return thresholds.value(pollutant, 0.007);
}

double POPsPage::getWarningThreshold(const QString& pollutant) {
  static const QMap<QString, double> thresholds = {
      {"PCBs", 0.005},
      {"Dioxins", 0.005},
      {"DDT", 0.005},
      {"Endrin", 0.005},
      {"Aldrin", 0.005},
      {"Dieldrin", 0.005},
      {"Other POPs", 0.005}};
  return thresholds.value(pollutant, 0.005);
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
      int(120 * qualityScore),  // Hue (120 for green, reduces towards red)
      200,                      // Saturation
      200                       // Value
      ));
  indicator->setBorderColor(Qt::transparent);

  chart->addSeries(indicator);

  if (chart->axes().size() >= 2) {
    indicator->attachAxis(chart->axes().at(0));
    indicator->attachAxis(chart->axes().at(1));
  }
}

bool POPsPage::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::ToolTip) {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
    QPointF pos = chartView->mapToScene(helpEvent->pos());
    QPointF chartPos = chart->mapFromScene(pos);

    // Get current time range
    QString timeRange = timeRangeSelector->currentText();
    QDateTime endDate = QDateTime::currentDateTime();
    QDateTime startDate;

    // Set time range based on selection
    if (timeRange == tr("January 2024")) {
      startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
      endDate = QDateTime::fromString("2024-01-31 23:59:59", "yyyy-MM-dd HH:mm:ss");
    } else if (timeRange == tr("February 2024")) {
      startDate = QDateTime::fromString("2024-02-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
      endDate = QDateTime::fromString("2024-02-29 23:59:59", "yyyy-MM-dd HH:mm:ss");
    }  // ... repeat for other months
    else if (timeRange == tr("Full Year 2024")) {
      startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
      endDate = QDateTime::fromString("2024-09-30 23:59:59", "yyyy-MM-dd HH:mm:ss");
    }

    // Find nearest data point within the selected time range
    qreal minDistance = std::numeric_limits<qreal>::max();
    ProcessedDataPoint nearestDataPoint;
    bool foundPoint = false;
    QString selectedPollutant = pollutantSelector->currentText();

    for (const auto& dataPoint : processedData) {
      // Check if point is within selected time range and pollutant type
      if (dataPoint.dateTime >= startDate &&
          dataPoint.dateTime <= endDate &&
          dataPoint.pollutantType == selectedPollutant) {
        QPointF pointPos = chart->mapToPosition(
            QPointF(dataPoint.dateTime.toMSecsSinceEpoch(), dataPoint.value));

        qreal distance = QLineF(chartPos, pointPos).length();

        if (distance < minDistance) {
          minDistance = distance;
          nearestDataPoint = dataPoint;
          foundPoint = true;
        }
      }
    }

    // Show tooltip if point is close enough
    if (foundPoint && minDistance < 50) {
      QString tooltipText = QString(
                                tr("Date: %1\n"
                                   "Value: %2 μg/L\n"
                                   "Location: %3\n"
                                   "Quality Score: %4"))
                                .arg(nearestDataPoint.dateTime.toString("yyyy-MM-dd HH:mm"))
                                .arg(nearestDataPoint.value, 0, 'f', 3)
                                .arg(nearestDataPoint.samplingPoint)
                                .arg(nearestDataPoint.qualityScore, 0, 'f', 2);

      // Add warning indicators if value exceeds thresholds
      double warningLevel = getWarningThreshold(pollutantSelector->currentText());
      double dangerLevel = getDangerThreshold(pollutantSelector->currentText());

      if (nearestDataPoint.value >= dangerLevel) {
        tooltipText += tr("\nStatus: DANGER - Exceeds safety threshold");
      } else if (nearestDataPoint.value >= warningLevel) {
        tooltipText += tr("\nStatus: WARNING - Approaching unsafe levels");
      } else {
        tooltipText += tr("\nStatus: Safe - Within acceptable limits");
      }

      if (nearestDataPoint.belowDetectionLimit) {
        tooltipText += tr("\nNote: Below detection limit");
      }

      QToolTip::setFont(QFont("Arial", 10));
      QToolTip::showText(helpEvent->globalPos(), tooltipText);
    } else {
      QToolTip::hideText();
    }

    return true;
  }
  return BasePage::eventFilter(obj, event);
}

void POPsPage::showSuccessMessage(const QString& message) {
  QMessageBox messageBox;
  messageBox.setObjectName("successBox");
  messageBox.setIcon(QMessageBox::Information);
  messageBox.setText(message);
  messageBox.exec();
}

void POPsPage::showErrorMessage(const QString& message) {
  QMessageBox messageBox;
  messageBox.setObjectName("errorBox");
  messageBox.setIcon(QMessageBox::Critical);
  messageBox.setText(message);
  messageBox.exec();
}

void POPsPage::handleExport() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export Data"), "", tr("CSV Files (*.csv)"));
  if (fileName.isEmpty()) {
    return;
  }

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    showErrorMessage(tr("Failed to create export file"));
    return;
  }

  QTextStream stream(&file);

  // Write headers
  stream << "@id,sample.samplingPoint.notation,sample.samplingPoint.label,"
         << "sample.sampleDateTime,determinand.label,determinand.definition,"
         << "determinand.notation,result,determinand.unit.label,"
         << "sample.sampledMaterialType.label,sample.purpose.label,"
         << "sample.samplingPoint.easting,sample.samplingPoint.northing\n";

  QString selectedPollutant = pollutantSelector->currentText();
  QString selectedTimeRange = timeRangeSelector->currentText();

  // Calculate date range based on selected month
  QDateTime startDate, endDate;

  if (selectedTimeRange == tr("Full Year 2024")) {
    startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString("2024-12-31 23:59:59", "yyyy-MM-dd HH:mm:ss");
  } else {
    QMap<QString, int> monthMap = {
        {tr("January 2024"), 1}, {tr("February 2024"), 2}, {tr("March 2024"), 3}, {tr("April 2024"), 4}, {tr("May 2024"), 5}, {tr("June 2024"), 6}, {tr("July 2024"), 7}, {tr("August 2024"), 8}, {tr("September 2024"), 9}};

    int month = monthMap[selectedTimeRange];
    int lastDay = QDate(2024, month, 1).daysInMonth();
    startDate = QDateTime::fromString(QString("2024-%1-01 00:00:00").arg(month, 2, 10, QChar('0')),
                                      "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString(QString("2024-%1-%2 23:59:59").arg(month, 2, 10, QChar('0')).arg(lastDay, 2, 10, QChar('0')),
                                    "yyyy-MM-dd HH:mm:ss");
  }

  int exportCount = 0;
  for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {
    // Check pollutant match
    QString pollutant = model->data(model->index(row, 4), Qt::DisplayRole).toString();
    if (pollutant != selectedPollutant) {
      continue;
    }

    // Check date range
    QString dateStr = model->data(model->index(row, 3), Qt::DisplayRole).toString();
    QDateTime sampleDate = QDateTime::fromString(dateStr, Qt::ISODate);
    if (!sampleDate.isValid() || sampleDate < startDate || sampleDate > endDate) {
      continue;
    }

    // Write all columns for matching row
    for (int col = 0; col < model->columnCount(QModelIndex()); ++col) {
      if (col > 0) {
        stream << ",";
      }
      stream << model->data(model->index(row, col), Qt::DisplayRole).toString();
    }
    stream << "\n";
    exportCount++;
  }

  file.close();

  showSuccessMessage(QString(tr("Successfully exported %1 records")).arg(exportCount));
}

void POPsPage::setupLocationSelector() {
  QVBoxLayout* locationSelectorVLabel = new QVBoxLayout();
  QLabel* locationLabel = new QLabel(tr("Location:"));
  locationSelector = new QComboBox();
  locationSelector->setMaximumWidth(250);
  locationSelector->setPlaceholderText(tr("Select Location"));
  locationSelectorVLabel->addWidget(locationLabel);
  locationSelectorVLabel->addWidget(locationSelector);

  controlsLayout->addLayout(locationSelectorVLabel);

  connect(locationSelector, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
          this, &POPsPage::filterDataByLocation);
}

void POPsPage::updateLocationSelector(const QString& selectedPollutant) {
  locationSelector->blockSignals(true);
  locationSelector->clear();

  QSet<QString> relevantLocations;
  QString selectedTimeRange = timeRangeSelector->currentText();

  // Calculate date range
  QDateTime startDate, endDate;
  if (selectedTimeRange == tr("Full Year 2024")) {
    startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString("2024-12-31 23:59:59", "yyyy-MM-dd HH:mm:ss");
  } else {
    QMap<QString, int> monthMap = {
        {tr("January 2024"), 1}, {tr("February 2024"), 2}, {tr("March 2024"), 3}, {tr("April 2024"), 4}, {tr("May 2024"), 5}, {tr("June 2024"), 6}, {tr("July 2024"), 7}, {tr("August 2024"), 8}, {tr("September 2024"), 9}};

    int month = monthMap[selectedTimeRange];
    int lastDay = QDate(2024, month, 1).daysInMonth();
    startDate = QDateTime::fromString(
        QString("2024-%1-01 00:00:00").arg(month, 2, 10, QChar('0')),
        "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString(
        QString("2024-%1-%2 23:59:59").arg(month, 2, 10, QChar('0')).arg(lastDay),
        "yyyy-MM-dd HH:mm:ss");
  }

  // Find locations with data in the selected time range
  for (const auto& point : processedData) {
    if (point.pollutantType == selectedPollutant &&
        point.dateTime >= startDate &&
        point.dateTime <= endDate) {
      relevantLocations.insert(point.samplingPoint);
    }
  }

  // Add "All Locations" option - grey it out if no data exists
  QStandardItem* allLocationsItem = new QStandardItem(tr("All Locations"));
  if (relevantLocations.isEmpty()) {
    allLocationsItem->setFlags(allLocationsItem->flags() & ~Qt::ItemIsEnabled);
    allLocationsItem->setData(QColor(128, 128, 128), Qt::ForegroundRole);
  }
  qobject_cast<QStandardItemModel*>(locationSelector->model())->appendRow(allLocationsItem);

  // Add individual locations
  for (const QString& location : availableLocations) {
    QStandardItem* item = new QStandardItem(location);
    if (!relevantLocations.contains(location)) {
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      item->setData(QColor(128, 128, 128), Qt::ForegroundRole);
    }
    qobject_cast<QStandardItemModel*>(locationSelector->model())->appendRow(item);
  }

  locationSelector->blockSignals(false);
  locationSelector->setCurrentText(tr("All Locations"));
}

void POPsPage::filterDataByLocation(const QString& location) {
  if (!chart) return;

  currentLevelSeries->clear();
  QString selectedPollutant = pollutantSelector->currentText();
  QString timeRange = timeRangeSelector->currentText();

  // Calculate date range
  QDateTime startDate, endDate;
  if (timeRange == tr("Full Year 2024")) {
    startDate = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString("2024-12-31 23:59:59", "yyyy-MM-dd HH:mm:ss");
  } else {
    QMap<QString, int> monthMap = {
        {tr("January 2024"), 1}, {tr("February 2024"), 2}, {tr("March 2024"), 3}, {tr("April 2024"), 4}, {tr("May 2024"), 5}, {tr("June 2024"), 6}, {tr("July 2024"), 7}, {tr("August 2024"), 8}, {tr("September 2024"), 9}};

    int month = monthMap[timeRange];
    int lastDay = QDate(2024, month, 1).daysInMonth();
    startDate = QDateTime::fromString(
        QString("2024-%1-01 00:00:00").arg(month, 2, 10, QChar('0')),
        "yyyy-MM-dd HH:mm:ss");
    endDate = QDateTime::fromString(
        QString("2024-%1-%2 23:59:59").arg(month, 2, 10, QChar('0')).arg(lastDay),
        "yyyy-MM-dd HH:mm:ss");
  }

  // Filter and sort data points
  QVector<QPair<qint64, double>> sortedPoints;
  double maxValue = 0;

  for (const auto& point : processedData) {
    if (point.dateTime >= startDate &&
        point.dateTime <= endDate &&
        point.pollutantType == selectedPollutant &&
        (location == tr("All Locations") || point.samplingPoint == location)) {
      sortedPoints.append({point.dateTime.toMSecsSinceEpoch(), point.value});
      maxValue = qMax(maxValue, point.value);
    }
  }

  // Sort points by time
  std::sort(sortedPoints.begin(), sortedPoints.end());

  // Add sorted points to series
  for (const auto& point : sortedPoints) {
    currentLevelSeries->append(point.first, point.second);
  }

  // Update chart
  updateChartAxes(startDate, endDate, maxValue);
  updateThresholds(selectedPollutant);
  updateSafetyIndicator();

  // Update chart title to include location
  QString locationText = (location == tr("All Locations")) ? "" : QString(tr(" at %1")).arg(location);
  chart->setTitle(QString(tr("%1 Concentration Trends%2 (%3)"))
                      .arg(selectedPollutant)
                      .arg(locationText)
                      .arg(timeRange));
}
