#include "trendsoverview.h"

#include <QCollator>
#include <QDateTime>
#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QToolTip>
#include <QtCharts>

#include "styles.h"

TrendsOverviewPage::TrendsOverviewPage(QWidget* parent)
    : BasePage("Pollutants Overview", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/trendsoverview.qss"}));
  setupUI();
  populatePollutants();
}

void TrendsOverviewPage::setupUI() {
  model.updateFromFile("../dataset/Y-2024-M.csv");

  pageLayout = new QVBoxLayout();

  contentFrame = new QFrame();
  contentFrame->setObjectName("contentFrame");
  QVBoxLayout* contentLayout = new QVBoxLayout(contentFrame);

  // Create horizontal layout for controls and stats
  QHBoxLayout* topSectionLayout = new QHBoxLayout();

  // Left side - Controls
  QFrame* controlsFrame = new QFrame();
  controlsFrame->setObjectName("controlsFrame");
  QVBoxLayout* controlsLayout = new QVBoxLayout(controlsFrame);

  QVBoxLayout* searchLayout = new QVBoxLayout();
  setupSearchControls(searchLayout);
  controlsLayout->addLayout(searchLayout);

  QHBoxLayout* thresholdLayout = new QHBoxLayout();
  setupThresholdIndicators(thresholdLayout);
  // controlsLayout->addStretch();
  controlsLayout->addLayout(thresholdLayout);

  topSectionLayout->addWidget(controlsFrame);

  // Right side - Info Cards
  QFrame* statsFrame = new QFrame();
  QHBoxLayout* horizontalStatsContainer = new QHBoxLayout(statsFrame);
  QVBoxLayout* leftStatsLayout = new QVBoxLayout();
  QVBoxLayout* rightStatsLayout = new QVBoxLayout();
  setupStatsSection(leftStatsLayout, rightStatsLayout);
  horizontalStatsContainer->addLayout(leftStatsLayout);
  horizontalStatsContainer->addLayout(rightStatsLayout);
  topSectionLayout->addWidget(statsFrame);

  // Add the top section to the main layout
  contentLayout->addLayout(topSectionLayout);

  // Add chart section
  setupChartSection(contentLayout);

  pageLayout->addWidget(contentFrame);
  contentArea->setLayout(pageLayout);
}

void TrendsOverviewPage::setupStatsSection(QVBoxLayout* leftLayout, QVBoxLayout* rightLayout) {
  addInfoCard(leftLayout, "Average", "0.0 µg/L");
  addInfoCard(leftLayout, "Last Reading", "0.0 µg/L");
  addInfoCard(rightLayout, "Minimum", "0.0 µg/L");
  addInfoCard(rightLayout, "Maximum", "0.0 µg/L");
}

void TrendsOverviewPage::setupControlsSection(QVBoxLayout* parentLayout) {
  QFrame* controlsFrame = new QFrame();
  controlsFrame->setObjectName("controlsFrame");
  QVBoxLayout* controlsLayout = new QVBoxLayout(controlsFrame);

  QVBoxLayout* searchLayout = new QVBoxLayout();
  setupSearchControls(searchLayout);
  controlsLayout->addLayout(searchLayout);

  QHBoxLayout* thresholdLayout = new QHBoxLayout();
  setupThresholdIndicators(thresholdLayout);
  controlsLayout->addLayout(thresholdLayout);

  parentLayout->addWidget(controlsFrame);
}

void TrendsOverviewPage::setupSearchControls(QVBoxLayout* layout) {
  QLabel* pollutantLabel = new QLabel("Search Pollutant:");
  pollutantSearch = new QLineEdit();
  pollutantSearch->setPlaceholderText("Type to search pollutants...");

  QLabel* locationLabel = new QLabel("Select Location:");
  locationSelector = new QComboBox();
  locationSelector->setObjectName("locationDropdown");
  locationSelector->setEnabled(false);

  layout->addWidget(pollutantLabel);
  layout->addWidget(pollutantSearch);
  layout->addWidget(locationLabel);
  layout->addWidget(locationSelector);

  connect(locationSelector, &QComboBox::currentTextChanged,
          this, &TrendsOverviewPage::onLocationChanged);
}

void TrendsOverviewPage::setupChartSection(QVBoxLayout* parentLayout) {
  QFrame* chartFrame = new QFrame();
  chartFrame->setObjectName("chartFrame");
  QVBoxLayout* chartLayout = new QVBoxLayout(chartFrame);

  chart = new QChart();
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->setTheme(QChart::ChartThemeLight);

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumHeight(500);

  chartLayout->addWidget(chartView);
  parentLayout->addWidget(chartFrame);

  setupChart();
}

void TrendsOverviewPage::addInfoCard(QVBoxLayout* layout, const QString& title,
                                     const QString& value) {
  QFrame* card = new QFrame();
  card->setProperty("class", "infoCard");

  QVBoxLayout* cardLayout = new QVBoxLayout(card);

  QLabel* titleLabel = new QLabel(title);
  titleLabel->setProperty("class", "infoTitle");

  QLabel* valueLabel = new QLabel(value);
  valueLabel->setProperty("class", "infoValue");
  valueLabel->setObjectName(title.toLower().remove(" ") + "Value");

  cardLayout->addWidget(titleLabel);
  cardLayout->addWidget(valueLabel);
  layout->addWidget(card);
}

void TrendsOverviewPage::setupThresholdIndicators(QHBoxLayout* layout) {
  layout->addWidget(createThresholdIndicator("Safe", "≤ 5.0 µg/L", "safeIndicator"));
  layout->addWidget(createThresholdIndicator("Warning", "5.1-7.0 µg/L", "warningIndicator"));
  layout->addWidget(createThresholdIndicator("Danger", "> 7.0 µg/L", "dangerIndicator"));
  layout->addStretch();
}

QFrame* TrendsOverviewPage::createThresholdIndicator(const QString& label,
                                                     const QString& range, const QString& objectName) {
  QFrame* indicator = new QFrame();
  indicator->setObjectName(objectName);
  indicator->setProperty("class", "thresholdIndicator");

  QHBoxLayout* layout = new QHBoxLayout(indicator);

  QLabel* titleLabel = new QLabel(label);
  titleLabel->setProperty("class", "thresholdLabel");

  QLabel* rangeLabel = new QLabel(range);
  rangeLabel->setProperty("class", "thresholdLabel");

  layout->addWidget(titleLabel);
  layout->addWidget(rangeLabel);

  return indicator;
}

void TrendsOverviewPage::setupChart() {
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  series = new QLineSeries();
  series->setPointsVisible(true);
  series->setPointLabelsVisible(true);

  connect(series, &QLineSeries::hovered,
          this, &TrendsOverviewPage::onPointHovered);

  chart->addSeries(series);

  axisX = new QDateTimeAxis;
  axisX->setTitleText("Time");
  axisX->setFormat("MMM yyyy");
  chart->addAxis(axisX, Qt::AlignBottom);

  axisY = new QValueAxis;
  axisY->setTitleText("Concentration (µg/L)");
  axisY->setLabelFormat("%.2f");
  chart->addAxis(axisY, Qt::AlignLeft);

  series->attachAxis(axisX);
  series->attachAxis(axisY);

  updateChart();
}

void TrendsOverviewPage::onPointHovered(const QPointF& point, bool state) {
  if (!state) return;

  QDateTime datetime = QDateTime::fromMSecsSinceEpoch(point.x());
  double value = point.y();

  QString tooltip = QString(
                        "Date: %1\n"
                        "Value: %2 µg/L\n"
                        "Status: %3")
                        .arg(datetime.toString("dd MMM yyyy"))
                        .arg(value, 0, 'f', 2)
                        .arg(getComplianceStatus(value));

  QToolTip::showText(QCursor::pos(), tooltip);
}

QString TrendsOverviewPage::getComplianceStatus(double value) const {
  if (value <= SAFE_THRESHOLD) return "Safe";
  if (value <= WARNING_THRESHOLD) return "Warning - Approaching Limit";
  return "Danger - Exceeds Safety Limit";
}

QColor TrendsOverviewPage::getComplianceColor(double value) const {
  if (value <= SAFE_THRESHOLD) return QColor("#4caf50");
  if (value <= WARNING_THRESHOLD) return QColor("#ff9800");
  return QColor("#f44336");
}

void TrendsOverviewPage::updateChart() {
  if (!model.hasData() || currentPollutant.isEmpty() || currentLocation.isEmpty()) {
    return;
  }

  chart->removeAllSeries();
  auto dataPoints = collectChartData();

  if (dataPoints.isEmpty()) {
    return;
  }

  series = new QLineSeries();
  series->setName(currentPollutant);
  series->setPointsVisible(true);

  // Add data points
  for (auto it = dataPoints.constBegin(); it != dataPoints.constEnd(); ++it) {
    series->append(it.key().toMSecsSinceEpoch(), it.value());
  }

  // Configure chart
  chart->addSeries(series);
  configureAxes(dataPoints);
  addSafetyThresholdLines();

  // Update title
  chart->setTitle(QString("%1 Concentration at %2")
                      .arg(currentPollutant)
                      .arg(currentLocation));

  // Connect signals
  connect(series, &QLineSeries::hovered,
          this, &TrendsOverviewPage::onPointHovered);

  series->attachAxis(axisX);
  series->attachAxis(axisY);
}

QMap<QDateTime, double> TrendsOverviewPage::collectChartData() const {
  QMap<QDateTime, double> dataPoints;

  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    if (model.data(model.index(row, 4), Qt::DisplayRole).toString() != currentPollutant ||
        model.data(model.index(row, 2), Qt::DisplayRole).toString() != currentLocation) {
      continue;
    }

    QString dateStr = model.data(model.index(row, 3), Qt::DisplayRole).toString();
    QDateTime datetime = QDateTime::fromString(dateStr, Qt::ISODate);

    bool ok;
    QString resultStr = model.data(model.index(row, 7), Qt::DisplayRole).toString();
    double value = resultStr.toDouble(&ok);

    if (ok && datetime.isValid()) {
      dataPoints[datetime] = value;
    }
  }

  return dataPoints;
}
void TrendsOverviewPage::configureAxes(const QMap<QDateTime, double>& dataPoints) {
  // Configure X axis (time)
  if (dataPoints.size() == 1) {
    QDateTime pointDate = dataPoints.firstKey();
    axisX->setRange(
        pointDate.addMonths(-1),
        pointDate.addMonths(1));
    axisX->setTickCount(3);
  } else {
    axisX->setRange(dataPoints.firstKey(), dataPoints.lastKey());
    axisX->setTickCount(std::min(8, static_cast<int>(dataPoints.size()) + 2));
  }

  // Configure Y axis (values)
  double minValue = std::numeric_limits<double>::max();
  double maxValue = std::numeric_limits<double>::lowest();

  for (double value : dataPoints.values()) {
    minValue = std::min(minValue, value);
    maxValue = std::max(maxValue, value);
  }

  // Handle single value or equal values
  if (minValue == maxValue) {
    double value = minValue;
    minValue = value * 0.8;  // 20% below
    maxValue = value * 1.2;  // 20% above
    if (minValue == 0 && maxValue == 0) {
      minValue = -1.0;
      maxValue = 1.0;
    }
  }

  // Add padding to Y axis
  double padding = (maxValue - minValue) * AXIS_PADDING_FACTOR;
  axisY->setRange(minValue - padding, maxValue + padding);
}

void TrendsOverviewPage::addSafetyThresholdLines() {
  // Add safe threshold line
  QLineSeries* safeLine = new QLineSeries();
  safeLine->setName("Safe Threshold");
  safeLine->setPen(QPen(QColor("#4caf50"), 1, Qt::DashLine));
  safeLine->append(axisX->min().toMSecsSinceEpoch(), SAFE_THRESHOLD);
  safeLine->append(axisX->max().toMSecsSinceEpoch(), SAFE_THRESHOLD);
  chart->addSeries(safeLine);
  safeLine->attachAxis(axisX);
  safeLine->attachAxis(axisY);

  // Add warning threshold line
  QLineSeries* warningLine = new QLineSeries();
  warningLine->setName("Warning Threshold");
  warningLine->setPen(QPen(QColor("#ff9800"), 1, Qt::DashLine));
  warningLine->append(axisX->min().toMSecsSinceEpoch(), WARNING_THRESHOLD);
  warningLine->append(axisX->max().toMSecsSinceEpoch(), WARNING_THRESHOLD);
  chart->addSeries(warningLine);
  warningLine->attachAxis(axisX);
  warningLine->attachAxis(axisY);
}

void TrendsOverviewPage::populatePollutants() {
  QSet<QString> pollutantSet;
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    QString pollutant = model.data(model.index(row, 4), Qt::DisplayRole).toString();
    if (!pollutant.isEmpty()) {
      pollutantSet.insert(pollutant);
    }
  }

  pollutants = pollutantSet.values();

  // Sort using natural sorting
  QCollator collator;
  collator.setNumericMode(true);
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  std::sort(pollutants.begin(), pollutants.end(),
            [&collator](const QString& s1, const QString& s2) {
              return collator.compare(s1, s2) < 0;
            });

  // Setup completer
  pollutantCompleter = new QCompleter(pollutants, this);
  pollutantCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  pollutantCompleter->setFilterMode(Qt::MatchContains);
  pollutantCompleter->setMaxVisibleItems(10);

  // Setup popup for completer to style it
  QAbstractItemView* popup = pollutantCompleter->popup();
  popup->setObjectName("searchPopup");
  // These styles cannot be externally loaded
  popup->setStyleSheet(R"(
        QAbstractItemView#searchPopup {
            border-radius: 4px;
            selection-background-color: rgba(64, 186, 213, 0.15);
            selection-color: white;
        }
        QAbstractItemView#searchPopup::item {
            padding: 4px 8px;
        }
    )");

  pollutantSearch->setCompleter(pollutantCompleter);

  connect(pollutantCompleter,
          QOverload<const QString&>::of(&QCompleter::activated),
          this, &TrendsOverviewPage::onPollutantSelected);
}

void TrendsOverviewPage::onPollutantSelected(const QString& pollutant) {
  currentPollutant = pollutant;
  updateLocations();
  locationSelector->setEnabled(true);
}

void TrendsOverviewPage::updateLocations() {
  if (currentPollutant.isEmpty()) {
    return;
  }

  locationSelector->clear();
  QSet<QString> locationSet;

  // Get locations for selected pollutant
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    QString determinandLabel = model.data(model.index(row, 4), Qt::DisplayRole).toString();
    if (determinandLabel == currentPollutant) {
      QString location = model.data(model.index(row, 2), Qt::DisplayRole).toString();
      if (!location.isEmpty()) {
        locationSet.insert(location);
      }
    }
  }

  // Convert to sorted list
  QStringList locations = locationSet.values();
  QCollator collator;
  collator.setNumericMode(true);
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  std::sort(locations.begin(), locations.end(),
            [&collator](const QString& s1, const QString& s2) {
              return collator.compare(s1, s2) < 0;
            });

  locationSelector->addItems(locations);

  // Set first location as default if available
  if (!locations.isEmpty()) {
    currentLocation = locations.first();
    locationSelector->setCurrentText(currentLocation);
    updateChart();
    updateStats();
  }
}

void TrendsOverviewPage::onLocationChanged(const QString& location) {
  currentLocation = location;
  updateChart();
  updateStats();
}

void TrendsOverviewPage::updateStats() {
  if (!model.hasData() || currentPollutant.isEmpty() || currentLocation.isEmpty()) {
    return;
  }

  double sum = 0.0;
  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::lowest();
  QDateTime lastDateTime;
  double lastValue = 0.0;
  int count = 0;

  // Calculate statistics
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    if (model.data(model.index(row, 4), Qt::DisplayRole).toString() != currentPollutant ||
        model.data(model.index(row, 2), Qt::DisplayRole).toString() != currentLocation) {
      continue;
    }

    bool ok;
    double value = model.data(model.index(row, 7), Qt::DisplayRole).toString().toDouble(&ok);
    if (!ok) continue;

    QDateTime datetime = QDateTime::fromString(
        model.data(model.index(row, 3), Qt::DisplayRole).toString(), Qt::ISODate);

    sum += value;
    min = std::min(min, value);
    max = std::max(max, value);
    count++;

    if (lastDateTime.isValid() && datetime > lastDateTime) {
      lastDateTime = datetime;
      lastValue = value;
    } else if (!lastDateTime.isValid()) {
      lastDateTime = datetime;
      lastValue = value;
    }
  }

  // Update info cards
  if (count > 0) {
    double avg = sum / count;
    findChild<QLabel*>("averageValue")->setText(QString::number(avg, 'f', 2) + " µg/L");
    findChild<QLabel*>("maximumValue")->setText(QString::number(max, 'f', 2) + " µg/L");
    findChild<QLabel*>("minimumValue")->setText(QString::number(min, 'f', 2) + " µg/L");
    findChild<QLabel*>("lastreadingValue")->setText(QString::number(lastValue, 'f', 2) + " µg/L");
  }
}