#include "dashboardpage.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "compliance.h"
#include "styles.h"
#include "trendsoverview.h"

DashboardPage::DashboardPage(SampleModel* model, QWidget* parent)
    : BasePage("Dashboard", parent), model(model) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/dashboard.qss"}));
  setupUI();
}

void DashboardPage::setupUI() {
  processData();

  QVBoxLayout* mainLayout = new QVBoxLayout(contentArea);

  // Add filter section
  QHBoxLayout* filterLayout = new QHBoxLayout();

  QLabel* timeFilterLabel = new QLabel(tr("Time Range:"));
  timeRangeFilter = new QComboBox();
  timeRangeFilter->addItem(tr("All Time"), AllTime);
  timeRangeFilter->addItem(tr("Last Month"), LastMonth);
  timeRangeFilter->addItem(tr("Last 3 Months"), LastThreeMonths);
  timeRangeFilter->addItem(tr("Last 6 Months"), LastSixMonths);

  connect(timeRangeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
            currentTimeRange = static_cast<TimeRange>(timeRangeFilter->itemData(index).toInt());
            refreshView();
          });

  filterLayout->addWidget(timeFilterLabel);
  filterLayout->addWidget(timeRangeFilter);
  filterLayout->addStretch();

  mainLayout->addLayout(filterLayout);

  // Cards grid
  cardsLayout = new QGridLayout();

  pollutantsCard = createPollutantsCard();
  popsCard = createPOPsCard();
  litterCard = createLitterCard();
  fluorinatedCard = createFluorinatedCard();
  complianceCard = createComplianceCard();

  cardsLayout->addWidget(pollutantsCard, 0, 0);
  cardsLayout->addWidget(popsCard, 0, 1);
  cardsLayout->addWidget(litterCard, 1, 0);
  cardsLayout->addWidget(fluorinatedCard, 1, 1);
  cardsLayout->addWidget(complianceCard, 2, 0, 1, 2);

  mainLayout->addLayout(cardsLayout);

  // Add footer
  QFrame* footer = createFooter();
  mainLayout->addWidget(footer);
  mainLayout->setAlignment(footer, Qt::AlignBottom);
}

void DashboardPage::refreshView() {
  processData();

  // Delete old widgets
  delete pollutantsCard;
  delete popsCard;
  delete litterCard;
  delete fluorinatedCard;
  delete complianceCard;

  // Create new widgets
  pollutantsCard = createPollutantsCard();
  popsCard = createPOPsCard();
  litterCard = createLitterCard();
  fluorinatedCard = createFluorinatedCard();
  complianceCard = createComplianceCard();

  // Add to layout
  cardsLayout->addWidget(pollutantsCard, 0, 0);
  cardsLayout->addWidget(popsCard, 0, 1);
  cardsLayout->addWidget(litterCard, 1, 0);
  cardsLayout->addWidget(fluorinatedCard, 1, 1);
  cardsLayout->addWidget(complianceCard, 2, 0, 1, 2);
}

void DashboardPage::processData() {
  QMap<QString, QVector<double>> commonPollutants;
  QMap<QString, QVector<double>> pops;
  QMap<QString, QVector<double>> litter;
  QMap<QString, QVector<double>> pfas;
  QSet<QString> locations;

  // Get current date for filtering
  QDateTime currentDate = QDateTime::currentDateTime();
  QDateTime filterDate;

  switch (currentTimeRange) {
    case LastMonth:
      filterDate = currentDate.addMonths(-1);
      break;
    case LastThreeMonths:
      filterDate = currentDate.addMonths(-3);
      break;
    case LastSixMonths:
      filterDate = currentDate.addMonths(-6);
      break;
    case AllTime:
    default:
      filterDate = QDateTime();
  }

  // Process all rows
  for (int row = 0; row < model->rowCount(QModelIndex()); row++) {
    // Check date filter
    QString dateStr = model->data(model->index(row, 3), Qt::DisplayRole).toString();
    QDateTime sampleDate = QDateTime::fromString(dateStr, Qt::ISODate);

    if (!filterDate.isNull() && sampleDate < filterDate) {
      continue;
    }

    QString pollutant = model->data(model->index(row, 4), Qt::DisplayRole).toString();
    QString location = model->data(model->index(row, 2), Qt::DisplayRole).toString();
    QString result = model->data(model->index(row, 7), Qt::DisplayRole).toString();

    // Skip invalid data
    bool ok;
    double value = result.toDouble(&ok);
    if (!ok && result.startsWith('<')) {
      value = result.mid(1).toDouble(&ok) / 2.0;  // Handle "less than" values
    }
    if (!ok) continue;

    locations.insert(location);

    // Categorize pollutants
    if (pollutant.contains("PCB", Qt::CaseInsensitive) ||
        pollutant.contains("Dioxin", Qt::CaseInsensitive)) {
      pops[pollutant].append(value);
    } else if (pollutant.contains("PF", Qt::CaseInsensitive)) {
      pfas[pollutant].append(value);
    } else if (pollutant.contains("Plastic", Qt::CaseInsensitive) ||
               pollutant.contains("Sewage", Qt::CaseInsensitive)) {
      litter[pollutant].append(value);
    } else if (isOverviewPollutant(pollutant)) {
      commonPollutants[pollutant].append(value);
    }
  }

  // Calculate metrics for common pollutants
  commonPollutantMetrics.totalSamples = 0;
  commonPollutantMetrics.samplesAboveLimit = 0;
  double maxTotal = 0;
  QString maxPollutant;

  auto& compliance = Compliance::instance();

  for (auto it = commonPollutants.constBegin(); it != commonPollutants.constEnd(); ++it) {
    double total = 0;
    for (double value : it.value()) {
      total += value;
      commonPollutantMetrics.totalSamples++;
      if (value > compliance.getCriticalThreshold(it.key())) {
        commonPollutantMetrics.samplesAboveLimit++;
      }
    }
    if (total > maxTotal) {
      maxTotal = total;
      maxPollutant = it.key();
    }
    commonPollutantMetrics.averageLevel = total / it.value().size();
  }
  commonPollutantMetrics.mostCommon = maxPollutant;
  commonPollutantMetrics.trend = calculateTrend(commonPollutants.first());

  // Calculate POPs metrics
  popsMetrics.totalSamples = 0;
  double popsTotal = 0;
  for (const auto& values : pops) {
    for (double value : values) {
      popsTotal += value;
      popsMetrics.totalSamples++;
      if (value > compliance.getCriticalThreshold("POPs")) {
        popsMetrics.samplesAboveLimit++;
      }
    }
  }
  popsMetrics.averageLevel = popsTotal / popsMetrics.totalSamples;

  // Calculate PFAS metrics
  pfasMetrics.totalSamples = 0;
  pfasMetrics.samplesAboveLimit = 0;
  double pfasTotal = 0;
  for (const auto& values : pfas) {
    for (double value : values) {
      pfasTotal += value;
      pfasMetrics.totalSamples++;
      if (value > PFAS_WARNING) {
        pfasMetrics.samplesAboveLimit++;
      }
    }
  }
  if (pfasMetrics.totalSamples > 0) {
    pfasMetrics.averageLevel = pfasTotal / pfasMetrics.totalSamples;
    pfasMetrics.trend = calculateTrend(pfas.first());
  }

  // Calculate litter metrics
  litterMetrics.totalSamples = 0;
  litterMetrics.samplesAboveLimit = 0;
  double litterTotal = 0;
  for (const auto& values : litter) {
    for (double value : values) {
      litterTotal += value;
      litterMetrics.totalSamples++;
      if (value > LITTER_WARNING) {
        litterMetrics.samplesAboveLimit++;
      }
    }
  }
  if (litterMetrics.totalSamples > 0) {
    litterMetrics.averageLevel = litterTotal / litterMetrics.totalSamples;
    litterMetrics.trend = calculateTrend(litter.first());
  }

  // Calculate overall compliance
  int totalMeasurements = 0;
  int compliantMeasurements = 0;
  complianceMetrics.totalSites = locations.size();
  complianceMetrics.sitesNeedingAction = 0;

  // Determine compliance levels across all categories
  QMap<QString, double> locationCompliance;
  for (const QString& location : locations) {
    int locationTotal = 0;
    int locationCompliant = 0;

    // Check compliance for each category at this location
    // [Add compliance checking logic]

    double compliance = locationCompliant / (double)locationTotal;
    locationCompliance[location] = compliance;
    if (compliance < 0.8) {  // 80% threshold
      complianceMetrics.sitesNeedingAction++;
    }
  }

  complianceMetrics.overallCompliance = compliantMeasurements / (double)totalMeasurements * 100;
  complianceMetrics.sitesCompliant = complianceMetrics.totalSites - complianceMetrics.sitesNeedingAction;
}

QString DashboardPage::calculateTrend(const QVector<double>& values) {
  if (values.size() < 2) return "Insufficient Data";

  int increases = 0;
  int decreases = 0;
  for (int i = 1; i < values.size(); i++) {
    if (values[i] > values[i - 1])
      increases++;
    else if (values[i] < values[i - 1])
      decreases++;
  }

  if (increases > decreases * 2) return "Rising";
  if (decreases > increases * 2) return "Falling";
  return "Stable";
}

void DashboardPage::navigateToPage(int pageIndex) {
  emit pageChangeRequested(pageIndex);
}

QString DashboardPage::getComplianceStatus(double value, double warningThreshold, double dangerThreshold) {
  if (value < warningThreshold) {
    return "Safe - Within Limits";
  } else if (value < dangerThreshold) {
    return "Warning - Elevated Levels";
  }
  return "Danger - Exceeds Limits";
}

QColor DashboardPage::getStatusColor(double value, double warningThreshold, double dangerThreshold) {
  if (value < warningThreshold) {
    return QColor("#28a745");  // Green
  } else if (value < dangerThreshold) {
    return QColor("#ffc107");  // Yellow/Warning
  }
  return QColor("#dc3545");  // Red/Danger
}

QFrame* DashboardPage::createPollutantsCard() {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);

  QLabel* titleLabel = new QLabel(tr("Pollutants Overview"));
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel(tr("Summary of pollutant levels and trends"));
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  QString metricsText = QString(
                            tr("• Average Level: %1 µg/L\n"
                               "• Samples Above Limit: %2\n"
                               "• Most Common: %3\n"
                               "• Trend: %4"))
                            .arg(commonPollutantMetrics.averageLevel)
                            .arg(int(commonPollutantMetrics.samplesAboveLimit * 100.0 /
                                     commonPollutantMetrics.totalSamples))
                            .arg(commonPollutantMetrics.mostCommon)
                            .arg(commonPollutantMetrics.trend);

  QLabel* metricsLabel = new QLabel(metricsText);

  // Status indicator using real data
  QLabel* statusLabel = new QLabel(
      getComplianceStatus(commonPollutantMetrics.averageLevel,
                          POLLUTANTS_WARNING, POLLUTANTS_DANGER));
  statusLabel->setStyleSheet(
      QString("background-color: %1; color: white; padding: 8px; border-radius: 4px;")
          .arg(getStatusColor(commonPollutantMetrics.averageLevel,
                              POLLUTANTS_WARNING, POLLUTANTS_DANGER)
                   .name()));

  // Navigate button
  QPushButton* navButton = new QPushButton(tr("View Detailed Analysis"));
  navButton->setObjectName("linksButton");
  connect(navButton, &QPushButton::clicked,
          [this]() { navigateToPage(2); });  // Index 2 is Pollutants Overview

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(statusLabel);
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}

QFrame* DashboardPage::createPOPsCard() {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);

  QLabel* titleLabel = new QLabel(tr("Persistent Organic Pollutants (POPs)"));
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel(tr("Status of POPs in water supplies"));
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  QString metricsText = QString(
                            "• Average Level: %1 µg/L\n"
                            "• Above Threshold: %2%\n"
                            "• Trend: %3")
                            .arg(popsMetrics.averageLevel, 0, 'f', 4)
                            .arg(int(popsMetrics.samplesAboveLimit * 100.0 / popsMetrics.totalSamples))
                            .arg(popsMetrics.trend);

  QLabel* metricsLabel = new QLabel(metricsText);

  QLabel* statusLabel = new QLabel(
      getComplianceStatus(popsMetrics.averageLevel, POPS_WARNING, POPS_DANGER));
  statusLabel->setStyleSheet(
      QString("background-color: %1; color: white; padding: 8px; border-radius: 4px;")
          .arg(getStatusColor(popsMetrics.averageLevel, POPS_WARNING, POPS_DANGER).name()));

  // Navigate button
  QPushButton* navButton = new QPushButton(tr("View Detailed Analysis"));
  navButton->setObjectName("linksButton");
  connect(navButton, &QPushButton::clicked, [this]() { navigateToPage(4); });

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(statusLabel);
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}

QFrame* DashboardPage::createLitterCard() {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);

  QLabel* titleLabel = new QLabel(tr("Environmental Litter"));
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel(tr("Water litter levels and trends"));
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  QString metricsText = QString(
                            "• Average Level: %1 mg/L\n"
                            "• Sites Above Limit: %2\n"
                            "• Trend: %3")
                            .arg(litterMetrics.averageLevel, 0, 'f', 2)
                            .arg(int(litterMetrics.samplesAboveLimit * 100.0 / litterMetrics.totalSamples))
                            .arg(litterMetrics.trend);

  QLabel* metricsLabel = new QLabel(metricsText);

  QLabel* statusLabel = new QLabel(
      getComplianceStatus(litterMetrics.averageLevel, LITTER_WARNING, LITTER_DANGER));
  statusLabel->setStyleSheet(
      QString("background-color: %1; color: white; padding: 8px; border-radius: 4px;")
          .arg(getStatusColor(litterMetrics.averageLevel, LITTER_WARNING, LITTER_DANGER).name()));

  // Navigate button
  QPushButton* navButton = new QPushButton(tr("View Litter Analysis"));
  navButton->setObjectName("linksButton");
  connect(navButton, &QPushButton::clicked, [this]() { navigateToPage(5); });

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(statusLabel);
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}

QFrame* DashboardPage::createFluorinatedCard() {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);

  QLabel* titleLabel = new QLabel("Fluorinated Compounds");
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel("PFAS and related compound levels");
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  QString metricsText = QString(
                            "• Average Level: %1 µg/L\n"
                            "• Samples Over Limit: %2\n"
                            "• Trend: %3")
                            .arg(pfasMetrics.averageLevel, 0, 'f', 3)
                            .arg(int(pfasMetrics.samplesAboveLimit * 100.0 / pfasMetrics.totalSamples))
                            .arg(pfasMetrics.trend);

  QLabel* metricsLabel = new QLabel(metricsText);

  QLabel* statusLabel = new QLabel(
      getComplianceStatus(pfasMetrics.averageLevel, PFAS_WARNING, PFAS_DANGER));
  statusLabel->setStyleSheet(
      QString("background-color: %1; color: white; padding: 8px; border-radius: 4px;")
          .arg(getStatusColor(pfasMetrics.averageLevel, PFAS_WARNING, PFAS_DANGER).name()));

  QPushButton* navButton = new QPushButton("View Fluorinated Analysis");
  navButton->setObjectName("linksButton");
  connect(navButton, &QPushButton::clicked, [this]() { navigateToPage(3); });

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(statusLabel);
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}

QFrame* DashboardPage::createComplianceCard() {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);

  QLabel* titleLabel = new QLabel(tr("Overall Compliance Status"));
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel(tr("Summary of safety compliance across all categories"));
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  // Calculate compliance metrics using real thresholds
  const auto& compliance = Compliance::instance();
  int totalSamples = 0;
  int compliantSamples = 0;
  QSet<QString> locations;
  QMap<QString, int> violationCounts;

  for (int row = 0; row < model->rowCount(QModelIndex()); row++) {
    QString pollutant = model->data(model->index(row, 4), Qt::DisplayRole).toString();
    QString result = model->data(model->index(row, 7), Qt::DisplayRole).toString();
    QString location = model->data(model->index(row, 2), Qt::DisplayRole).toString();

    bool ok;
    double value = result.toDouble(&ok);
    if (!ok && result.startsWith('<')) {
      value = result.mid(1).toDouble(&ok) / 2.0;
    }
    if (!ok) continue;

    locations.insert(location);
    totalSamples++;

    if (compliance.isCompliant(pollutant, value)) {
      compliantSamples++;
    } else {
      violationCounts[pollutant]++;
    }
  }

  // Calculate metrics
  double complianceRate = totalSamples > 0 ? (compliantSamples * 100.0 / totalSamples) : 0;
  int sitesNeedingAction = 0;
  QString mainConcern = "None";
  int maxViolations = 0;

  for (auto it = violationCounts.begin(); it != violationCounts.end(); ++it) {
    if (it.value() > maxViolations) {
      maxViolations = it.value();
      mainConcern = it.key();
    }
    if (it.value() > 0) sitesNeedingAction++;
  }

  QString metricsText = QString(
                            tr("• Overall Compliance: %1%\n"
                               "• Sites Meeting Standards: %2/%3\n"
                               "• Main Concern: %4\n"
                               "• Locations Needing Action: %5"))
                            .arg(complianceRate, 0, 'f', 1)
                            .arg(locations.size() - sitesNeedingAction)
                            .arg(locations.size())
                            .arg(mainConcern)
                            .arg(sitesNeedingAction);

  QLabel* metricsLabel = new QLabel(metricsText);

  // Status indicator based on overall compliance rate
  QString statusText;
  QString statusColor;
  if (complianceRate >= 70) {
    statusText = "Good - High Compliance";
    statusColor = "#28a745";
  } else if (complianceRate >= 55) {
    statusText = "Warning - Moderate Compliance";
    statusColor = "#ffc107";
  } else {
    statusText = "Critical - Low Compliance";
    statusColor = "#dc3545";
  }

  QLabel* statusLabel = new QLabel(statusText);
  statusLabel->setStyleSheet(
      QString("background-color: %1; color: white; padding: 8px; border-radius: 4px;")
          .arg(statusColor));

  QPushButton* navButton = new QPushButton(tr("View Full Compliance Report"));
  navButton->setObjectName("linksButton");
  connect(navButton, &QPushButton::clicked, [this]() { navigateToPage(6); });

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(statusLabel);
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}

QFrame* DashboardPage::createFooter() {
  QFrame* footer = new QFrame();
  footer->setObjectName("dashboardFooter");

  QHBoxLayout* footerLayout = new QHBoxLayout(footer);

  // Add resource links
  auto addLink = [&](const QString& text, const QString& url) {
    QPushButton* link = new QPushButton(text);
    link->setObjectName("footerLink");
    link->setCursor(Qt::PointingHandCursor);
    connect(link, &QPushButton::clicked, [url]() {
      QDesktopServices::openUrl(QUrl(url));
    });
    footerLayout->addWidget(link);
  };

  addLink(tr("EU Water Guidelines"), "https://ec.europa.eu/environment/water/");
  addLink(tr("UK Water Standards"), "https://www.gov.uk/topic/environmental-management/water");
  addLink(tr("WHO Guidelines"), "https://www.who.int/teams/environment-climate-change-and-health/water-sanitation-and-health/");

  footerLayout->addStretch();  // Push links to the left

  return footer;
}

bool DashboardPage::isOverviewPollutant(const QString& pollutant) const {
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