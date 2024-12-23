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
  buildLocationCache();

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

void TrendsOverviewPage::loadDataset(const QString& filename) {
  model.updateFromFile(filename);
  buildLocationCache();  // Rebuild location cache
  populatePollutants();  // Repopulate pollutant list
  updateChart();         // Update chart with new data
  updateStats();         // Update statistics
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
  pollutantSearch = new SearchLineEdit();
  pollutantSearch->setPlaceholderText("Type to search pollutants...");

  QLabel* locationLabel = new QLabel("Select Location:");
  locationSelector = new QComboBox();
  locationSelector->setObjectName("locationDropdown");
  locationSelector->setStyleSheet("");  // Clear any existing styles
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
  chartView->setBackgroundBrush(Qt::transparent);        // Make background transparent
  chartView->setStyleSheet("background: transparent;");  // Ensure no background color

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
  // No units in the threshold indicators since they are relative values
  layout->addWidget(createThresholdIndicator("Safe", "≤ 5.0", "safeIndicator"));
  layout->addWidget(createThresholdIndicator("Warning", "5.1-7.0", "warningIndicator"));
  layout->addWidget(createThresholdIndicator("Danger", "> 7.0", "dangerIndicator"));
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
  chart->legend()->setFont(QFont("Arial", 10));

  // Set chart visual properties
  chart->setPlotAreaBackgroundVisible(true);
  chart->setPlotAreaBackgroundBrush(QBrush(QColor("#f8f9fa")));
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chart->setMargins(QMargins(0, 0, 0, 0));

  // Style the title
  QFont titleFont("Arial", 14, QFont::Bold);
  chart->setTitleFont(titleFont);
  chart->setTitleBrush(QBrush(QColor("#051826")));

  series = new QLineSeries();
  series->setPointsVisible(true);
  series->setPointLabelsVisible(false);        // Changed to false for cleaner look
  series->setPen(QPen(QColor("#40BAD5"), 2));  // Thicker, branded color line

  connect(series, &QLineSeries::hovered,
          this, &TrendsOverviewPage::onPointHovered);

  chart->addSeries(series);

  axisX = new QDateTimeAxis;
  axisX->setTitleText("Month");
  axisX->setTitleFont(QFont("Arial", 14, QFont::Medium));
  axisX->setLabelsFont(QFont("Arial", 12));
  axisX->setFormat("MMM yyyy");
  axisX->setGridLineVisible(true);
  axisX->setGridLineColor(QColor("#e9ecef"));
  axisX->setLabelsColor(QColor("#495057"));
  axisX->setLinePenColor(QColor("#ced4da"));
  chart->addAxis(axisX, Qt::AlignBottom);

  axisY = new QValueAxis;
  axisY->setTitleText("Concentration");
  axisY->setTitleFont(QFont("Arial", 14, QFont::Medium));
  axisY->setLabelsFont(QFont("Arial", 12));
  axisY->setLabelFormat("%.2f");
  axisY->setGridLineVisible(true);
  axisY->setGridLineColor(QColor("#e9ecef"));
  axisY->setLabelsColor(QColor("#495057"));
  axisY->setLinePenColor(QColor("#ced4da"));
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
                        "Value: %2 %3\n"
                        "Status: %4")
                        .arg(datetime.toString("dd MMM yyyy"))
                        .arg(value, 0, 'f', 2)
                        .arg(currentUnit)
                        .arg(getComplianceStatus(value));

  QToolTip::showText(QCursor::pos(), tooltip);
}

QString TrendsOverviewPage::getComplianceStatus(double value) const {
  if (value <= SAFE_THRESHOLD) return "Safe";
  if (value <= DANGER_THRESHOLD) return "Warning - Approaching Danger Limit";
  return "Danger - Exceeds Safety Limit";
}

QColor TrendsOverviewPage::getComplianceColor(double value) const {
  if (value <= SAFE_THRESHOLD) return QColor("#4caf50");
  if (value <= DANGER_THRESHOLD) return QColor("#ff9800");
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
  series->setPen(QPen(QColor("#40BAD5"), 2));
  series->setMarkerSize(4);

  // Add data points
  for (auto it = dataPoints.constBegin(); it != dataPoints.constEnd(); ++it) {
    series->append(it.key().toMSecsSinceEpoch(), it.value());
  }

  // Configure chart and axes
  chart->addSeries(series);
  configureAxes(dataPoints);
  addSafetyThresholdLines();

  // Update title with a more descriptive format
  chart->setTitle(QString("%1 Concentration Trends at\n%2")
                      .arg(currentPollutant)
                      .arg(currentLocation));

  // Connect hover event
  connect(series, &QLineSeries::hovered,
          this, &TrendsOverviewPage::onPointHovered);

  axisY->setTitleText(QString("Concentration (%1)").arg(currentUnit));

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
  safeLine->setName("Safe Level");
  safeLine->setPen(QPen(QColor("#4caf50"), 1, Qt::DashLine));
  safeLine->append(axisX->min().toMSecsSinceEpoch(), SAFE_THRESHOLD);
  safeLine->append(axisX->max().toMSecsSinceEpoch(), SAFE_THRESHOLD);
  chart->addSeries(safeLine);
  safeLine->attachAxis(axisX);
  safeLine->attachAxis(axisY);

  // Add danger threshold line
  QLineSeries* dangerLine = new QLineSeries();
  dangerLine->setName("Danger Level");
  dangerLine->setPen(QPen(QColor("red"), 1, Qt::DashLine));
  dangerLine->append(axisX->min().toMSecsSinceEpoch(), DANGER_THRESHOLD);
  dangerLine->append(axisX->max().toMSecsSinceEpoch(), DANGER_THRESHOLD);
  chart->addSeries(dangerLine);
  dangerLine->attachAxis(axisX);
  dangerLine->attachAxis(axisY);
}

bool TrendsOverviewPage::isOverviewPollutant(const QString& pollutant) const {
  // Heavy Metals
  static const QSet<QString> heavyMetals = {
      "Pb Filtered", "Hg Filtered", "Cd Filtered", "Cr- Filtered",
      "Cu Filtered", "Zn- Filtered", "As-Filtered", "Al- Filtered", "Ni- Filtered"};

  // Nutrients
  static const QSet<QString> nutrients = {
      "Nitrogen - N", "Nitrate-N", "Nitrite-N", "Orthophospht",
      "Phosphorus-P", "NH3 filt N", "N Oxidised"};

  // Volatile Organic Compounds
  static const QSet<QString> vocs = {
      "1,1,2-Trichloroethane", "Chloroform", "Trichloroeth", "Carbon Tet",
      "TetClEthene", "Benzene", "Toluene", "Ethylbenzene", "o-Xylene", "m-p-Xylene"};

  return heavyMetals.contains(pollutant) ||
         nutrients.contains(pollutant) ||
         vocs.contains(pollutant);
}

bool TrendsOverviewPage::hasResultData(const QString& pollutant, const QString& location) const {
  return validLocationCache.contains(pollutant) &&
         validLocationCache[pollutant].contains(location);
}

void TrendsOverviewPage::populatePollutants() {
  QSet<QString> pollutantSet;
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    QString pollutant = model.data(model.index(row, 4), Qt::DisplayRole).toString();
    if (!pollutant.isEmpty() && isOverviewPollutant(pollutant)) {
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

  // Setup popup for completer to style it
  QAbstractItemView* popup = pollutantCompleter->popup();
  popup->setObjectName("searchPopup");
  // These styles cannot be externally loaded
  popup->setStyleSheet(R"(
        QAbstractItemView#searchPopup {
            border-radius: 4px;
        }
        QAbstractItemView#searchPopup::item {
            padding: 4px 8px;
        }
    )");

  pollutantSearch->setCompleter(pollutantCompleter);

  // Connect focus event to show all completions
  connect(pollutantSearch, &SearchLineEdit::focusReceived,
          this, [this]() {
            if (pollutantSearch->text().isEmpty()) {
              pollutantSearch->setText("");    // Trigger completer popup
              pollutantCompleter->complete();  // Force the popup to show
            }
          });

  connect(pollutantCompleter,
          QOverload<const QString&>::of(&QCompleter::activated),
          this, &TrendsOverviewPage::onPollutantSelected);
}

void TrendsOverviewPage::onPollutantSelected(const QString& pollutant) {
  currentPollutant = pollutant;

  // Find the unit for this pollutant
  currentUnit = "";
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    if (model.data(model.index(row, 4), Qt::DisplayRole).toString() == pollutant) {
      currentUnit = model.data(model.index(row, 8), Qt::DisplayRole).toString();
      break;
    }
  }

  updateLocations();
  locationSelector->setEnabled(true);
}

void TrendsOverviewPage::updateLocations() {
  if (currentPollutant.isEmpty()) {
    return;
  }

  locationSelector->clear();

  // Get locations from cache
  QStringList locations;
  if (validLocationCache.contains(currentPollutant)) {
    locations = validLocationCache[currentPollutant].values();
  }

  // Sort locations
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

void TrendsOverviewPage::buildLocationCache() {
  validLocationCache.clear();

  // Pre-calculate all valid pollutant-location pairs
  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    QString pollutant = model.data(model.index(row, 4), Qt::DisplayRole).toString();
    if (!pollutant.isEmpty() && isOverviewPollutant(pollutant)) {
      QString location = model.data(model.index(row, 2), Qt::DisplayRole).toString();
      if (!location.isEmpty()) {
        QString resultStr = model.data(model.index(row, 7), Qt::DisplayRole).toString();
        bool ok;
        resultStr.toDouble(&ok);
        if (ok) {
          validLocationCache[pollutant].insert(location);
        }
      }
    }
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
    findChild<QLabel*>("averageValue")->setText(QString::number(avg, 'f', 2) + " " + currentUnit);
    findChild<QLabel*>("maximumValue")->setText(QString::number(max, 'f', 2) + " " + currentUnit);
    findChild<QLabel*>("minimumValue")->setText(QString::number(min, 'f', 2) + " " + currentUnit);
    findChild<QLabel*>("lastreadingValue")->setText(QString::number(lastValue, 'f', 2) + " " + currentUnit);
  }
}