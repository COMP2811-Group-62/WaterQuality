#include "dashboardpage.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "styles.h"

DashboardPage::DashboardPage(QWidget* parent)
    : BasePage("Dashboard", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/dashboard.qss"}));
  setupUI();
}

void DashboardPage::setupUI() {
  model.updateFromFile("../dataset/Y-2024-M.csv");
  loadData();

  // Create main layout
  QVBoxLayout* mainLayout = new QVBoxLayout(contentArea);

  // Cards grid
  cardsLayout = new QGridLayout();

  cardsLayout->addWidget(createPollutantsCard(), 0, 0);
  cardsLayout->addWidget(createPOPsCard(), 0, 1);
  cardsLayout->addWidget(createLitterCard(), 1, 0);
  cardsLayout->addWidget(createFluorinatedCard(), 1, 1);
  cardsLayout->addWidget(createComplianceCard(), 2, 0, 1, 2);

  mainLayout->addLayout(cardsLayout);
}

void DashboardPage::loadData() {
  if (!model.hasData()) {
    qWarning() << "No data loaded in model";
    return;
  }

  // Maps to store data for different pollutant types
  QMap<QString, QVector<double>> commonPollutants;
  QMap<QString, QVector<double>> pops;
  QMap<QString, QVector<double>> litter;
  QMap<QString, QVector<double>> pfas;
  QSet<QString> locations;

  // Process all rows
  for (int row = 0; row < model.rowCount(QModelIndex()); row++) {
    QString pollutant = model.data(model.index(row, 4), Qt::DisplayRole).toString();
    QString location = model.data(model.index(row, 2), Qt::DisplayRole).toString();
    QString result = model.data(model.index(row, 7), Qt::DisplayRole).toString();

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
    } else if (pollutant.contains("Litter", Qt::CaseInsensitive) ||
               pollutant.contains("Debris", Qt::CaseInsensitive)) {
      litter[pollutant].append(value);
    } else if (pollutant.contains("Chloroform", Qt::CaseInsensitive) ||
               pollutant.contains("Trichloroethane", Qt::CaseInsensitive)) {
      commonPollutants[pollutant].append(value);
    }
  }

  // Calculate metrics for common pollutants
  commonPollutantMetrics.totalSamples = 0;
  commonPollutantMetrics.samplesAboveLimit = 0;
  double maxTotal = 0;
  QString maxPollutant;

  for (auto it = commonPollutants.constBegin(); it != commonPollutants.constEnd(); ++it) {
    double total = 0;
    for (double value : it.value()) {
      total += value;
      commonPollutantMetrics.totalSamples++;
      if (value > POLLUTANTS_WARNING) {
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
      if (value > POPS_WARNING) {
        popsMetrics.samplesAboveLimit++;
      }
    }
  }
  popsMetrics.averageLevel = popsTotal / popsMetrics.totalSamples;
  popsMetrics.trend = calculateTrend(pops.first());

  // Calculate PFAS metrics similarly
  // [Similar calculations for PFAS]

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
  emit dynamic_cast<QStackedWidget*>(parent())->setCurrentIndex(pageIndex);
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

  QLabel* titleLabel = new QLabel("Common Pollutants");
  titleLabel->setObjectName("cardTitle");

  QLabel* subtitleLabel = new QLabel("Overview of water pollutants and their levels");
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  // Add key metrics
  QString metricsText = QString(
                            "• Average Level: %1 µg/L\n"
                            "• Samples Above Limit: %2 \n"
                            "• Most Common: %3\n"
                            "• Trend: %4")
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
  QPushButton* navButton = new QPushButton("View Detailed Analysis");
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

  QLabel* titleLabel = new QLabel("Persistent Organic Pollutants (POPs)");
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel("Status of POPs in water supplies");
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

  QPushButton* navButton = new QPushButton("View POPs Analysis");
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

  QLabel* titleLabel = new QLabel("Environmental Litter");
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel("Water litter levels and trends");
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

  QPushButton* navButton = new QPushButton("View Litter Analysis");
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

  QLabel* titleLabel = new QLabel("Overall Compliance Status");
  titleLabel->setObjectName("cardTitle");
  QLabel* subtitleLabel = new QLabel("Summary of safety compliance across all categories");
  subtitleLabel->setObjectName("cardSubtitle");

  headerLayout->addWidget(titleLabel);
  headerLayout->addWidget(subtitleLabel);

  // Content
  QWidget* content = new QWidget;
  QVBoxLayout* contentLayout = new QVBoxLayout(content);

  QString metricsText = QString(
                            "• Overall Compliance: %1\n"
                            "• Sites Meeting Standards: %2/%3\n"
                            "• Main Concern: %4\n"
                            "• Locations Needing Action: %5")
                            .arg(complianceMetrics.overallCompliance, 0, 'f', 1)
                            .arg(complianceMetrics.sitesCompliant)
                            .arg(complianceMetrics.totalSites)
                            .arg(complianceMetrics.mainConcern)
                            .arg(complianceMetrics.sitesNeedingAction);

  QLabel* metricsLabel = new QLabel(metricsText);

  QPushButton* navButton = new QPushButton("View Full Compliance Report");
  connect(navButton, &QPushButton::clicked, [this]() { navigateToPage(6); });

  contentLayout->addWidget(metricsLabel);
  contentLayout->addStretch();
  contentLayout->addWidget(navButton);

  layout->addWidget(header);
  layout->addWidget(content);

  return card;
}