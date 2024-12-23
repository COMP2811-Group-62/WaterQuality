#include "compliancedashboard.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QSet>
#include <QString>
#include <QVBoxLayout>

#include "styles.h"

ComplianceDashboard::ComplianceDashboard(QWidget* parent)
    : BasePage("Compliance Dashboard", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/compliancedashboard.qss"}));
  setupUI();
  populateFilters();
  updateCards();
}

void ComplianceDashboard::setupUI() {
  // Main Layout
  pageLayout = new QVBoxLayout();

  // Set up filters section
  filtersFrame = new QFrame();
  filtersFrame->setObjectName("filtersFrame");
  filtersLayout = new QHBoxLayout(filtersFrame);
  filtersLayout->setSpacing(24);
  setUpFilters(filtersLayout);
  pageLayout->addWidget(filtersFrame);

  // Set up scrollable cards
  setUpScrollableCards();

  contentArea->setLayout(pageLayout);
}

void ComplianceDashboard::loadDataset(const QString& filename) {
  model.updateFromFile(filename);
  populateFilters();  // Repopulate filters with new data
  updateCards();      // Refresh compliance cards
}

void ComplianceDashboard::setUpFilters(QHBoxLayout* layout) {
  // Search Bar
  searchBar = new QLineEdit();
  searchBar->setPlaceholderText("Search pollutants...");
  searchBar->setObjectName("searchBar");

  locationLabel = new QLabel("Location:");
  locationSelect = new QComboBox();
  locationSelect->setObjectName("locationSelect");

  complianceLabel = new QLabel("Compliance Status:");
  complianceSelect = new QComboBox();
  complianceSelect->setObjectName("complianceSelect");

  complianceSelect->addItem("All");
  complianceSelect->addItem("Compliant");
  complianceSelect->addItem("Non-Compliant");

  layout->addWidget(searchBar);
  layout->addWidget(locationLabel);
  layout->addWidget(locationSelect);
  layout->addWidget(complianceLabel);
  layout->addWidget(complianceSelect);
  layout->addStretch();

  connect(searchBar, &QLineEdit::textChanged,
          this, &ComplianceDashboard::onPollutantSearched);
  connect(locationSelect, &QComboBox::currentTextChanged,
          this, &ComplianceDashboard::onLocationFiltered);
  connect(complianceSelect, &QComboBox::currentTextChanged,
          this, &ComplianceDashboard::onComplianceFiltered);
}

void ComplianceDashboard::setUpScrollableCards() {
  scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setObjectName("scrollArea");

  QWidget* scrollContent = new QWidget();
  cardsLayout = new QGridLayout(scrollContent);
  cardsLayout->setSpacing(12);
  cardsLayout->setContentsMargins(12, 12, 12, 12);

  scrollArea->setWidget(scrollContent);
  pageLayout->addWidget(scrollArea);
}

void ComplianceDashboard::populateFilters() {
  locationSelect->clear();
  QSet<QString> locationSet;
  QSet<QString> pollutantSet;

  for (int row = 0; row < model.rowCount(QModelIndex()); ++row) {
    QString location = model.data(model.index(row, 2), Qt::DisplayRole).toString();
    QString pollutant = model.data(model.index(row, 4), Qt::DisplayRole).toString();

    if (!location.isEmpty()) {
      locationSet.insert(location);
    }
    if (!pollutant.isEmpty()) {
      pollutantSet.insert(pollutant);
    }
  }

  locations = locationSet.values();
  pollutants = pollutantSet.values();

  locationSelect->addItem("All Locations");
  locationSelect->addItems(locations);
}

void ComplianceDashboard::createComplianceCard(QVBoxLayout* layout,
                                               const QString& pollutant,
                                               const QString& location,
                                               const QString& value,
                                               bool compliant) {
  QFrame* card = new QFrame();
  card->setObjectName("complianceCard");
  QVBoxLayout* cardLayout = new QVBoxLayout(card);

  // Pollutant Name and Value
  QLabel* nameLabel = new QLabel(pollutant);
  nameLabel->setObjectName("pollutantName");

  QLabel* locationLabel = new QLabel(QString("Location: %1").arg(location));
  locationLabel->setObjectName("locationLabel");

  QLabel* valueLabel = new QLabel(QString("Current Value: %1 µg/L").arg(value));
  valueLabel->setObjectName("valueLabel");

  // Compliance Indicator
  QLabel* complianceLabel = new QLabel(compliant ? "✓ Compliant" : "⚠ Non-Compliant");
  complianceLabel->setStyleSheet(compliant ? "color: #2ecc71; font-weight: bold;" : "color: #e74c3c; font-weight: bold;");
  complianceLabel->setObjectName("complianceLabel");

  // Threshold Display
  double threshold = getThresholdForPollutant(pollutant);
  QLabel* thresholdLabel = new QLabel(QString("Threshold: %1 µg/L").arg(threshold));
  thresholdLabel->setObjectName("thresholdLabel");

  // Details Button
  QPushButton* detailsBtn = new QPushButton("View Details");
  detailsBtn->setObjectName("detailsButton");
  connect(detailsBtn, &QPushButton::clicked, this, [=]() {
    showPollutantDetails(pollutant, location, value);
  });

  cardLayout->addWidget(nameLabel);
  cardLayout->addWidget(locationLabel);
  cardLayout->addWidget(valueLabel);
  cardLayout->addWidget(complianceLabel);
  cardLayout->addWidget(thresholdLabel);
  cardLayout->addWidget(detailsBtn);

  layout->addWidget(card);
}

double ComplianceDashboard::getThresholdForPollutant(const QString& pollutant) {
  return COMPLIANCE_THRESHOLDS.value(pollutant, 0.1);  // Default to 0.1 if not found
}

QString ComplianceDashboard::getTrendAnalysis(const QString& pollutant, const QString& location) {
  // In a real implementation, this would analyze historical data
  // For now, return a placeholder analysis
  return QString(
             "Historical Analysis for %1 at %2:\n\n"
             "• Past 3 months show stable levels\n"
             "• No significant spikes detected\n"
             "• Seasonal variations are within normal range")
      .arg(pollutant)
      .arg(location);
}

void ComplianceDashboard::populateCardContent(QVBoxLayout* cardLayout,
                                              const QString& pollutant,
                                              const QString& location,
                                              const QString& value,
                                              bool compliant) {
  // Pollutant Name and Value
  QLabel* nameLabel = new QLabel(pollutant);
  nameLabel->setObjectName("pollutantName");

  QLabel* locationLabel = new QLabel(QString("Location: %1").arg(location));
  locationLabel->setObjectName("locationLabel");

  QLabel* valueLabel = new QLabel(QString("Current Value: %1 µg/L").arg(value));
  valueLabel->setObjectName("valueLabel");

  // Compliance Indicator
  QLabel* complianceLabel = new QLabel(compliant ? "✓ Compliant" : "⚠ Non-Compliant");
  complianceLabel->setStyleSheet(compliant ? "color: #2ecc71; font-weight: bold;" : "color: #e74c3c; font-weight: bold;");
  complianceLabel->setObjectName("complianceLabel");

  // Threshold Display
  double threshold = getThresholdForPollutant(pollutant);
  QLabel* thresholdLabel = new QLabel(QString("Threshold: %1 µg/L").arg(threshold));
  thresholdLabel->setObjectName("thresholdLabel");

  // Details Button
  QPushButton* detailsBtn = new QPushButton("View Details");
  detailsBtn->setObjectName("detailsButton");
  connect(detailsBtn, &QPushButton::clicked, this, [=]() {
    showPollutantDetails(pollutant, location, value);
  });

  cardLayout->addWidget(nameLabel);
  cardLayout->addWidget(locationLabel);
  cardLayout->addWidget(valueLabel);
  cardLayout->addWidget(complianceLabel);
  cardLayout->addWidget(thresholdLabel);
  cardLayout->addWidget(detailsBtn);
}

void ComplianceDashboard::showPollutantDetails(const QString& pollutant,
                                               const QString& location,
                                               const QString& value) {
  QDialog* detailsDialog = new QDialog(this);
  detailsDialog->setWindowTitle(QString("%1 - Detailed Analysis").arg(pollutant));
  detailsDialog->setMinimumWidth(400);

  QVBoxLayout* dialogLayout = new QVBoxLayout(detailsDialog);

  // Current Status
  QLabel* statusLabel = new QLabel("Current Status");
  statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
  dialogLayout->addWidget(statusLabel);

  double threshold = getThresholdForPollutant(pollutant);
  bool isCompliant = value.toDouble() <= threshold;

  QString complianceInfo = isCompliant ? "✓ Currently within safe limits." : "⚠ Exceeds safety threshold. Immediate attention required.";

  QLabel* complianceLabel = new QLabel(complianceInfo);
  complianceLabel->setStyleSheet(isCompliant ? "color: #2ecc71;" : "color: #e74c3c;");
  complianceLabel->setWordWrap(true);
  dialogLayout->addWidget(complianceLabel);

  // Trend Analysis
  QLabel* trendTitle = new QLabel("Trend Analysis");
  trendTitle->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
  dialogLayout->addWidget(trendTitle);

  QLabel* trendLabel = new QLabel(getTrendAnalysis(pollutant, location));
  trendLabel->setWordWrap(true);
  dialogLayout->addWidget(trendLabel);

  // Recommendations
  QLabel* recomTitle = new QLabel("Recommendations");
  recomTitle->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
  dialogLayout->addWidget(recomTitle);

  QString recommendations = isCompliant ? "• Continue regular monitoring\n"
                                          "• Maintain current control measures\n"
                                          "• Schedule next review in 30 days"
                                        : "• Increase monitoring frequency\n"
                                          "• Review control measures\n"
                                          "• Prepare incident report\n"
                                          "• Schedule immediate follow-up testing";

  QLabel* recomLabel = new QLabel(recommendations);
  dialogLayout->addWidget(recomLabel);

  // Dialog Buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok,
      Qt::Horizontal,
      detailsDialog);
  connect(buttonBox, &QDialogButtonBox::accepted,
          detailsDialog, &QDialog::accept);
  dialogLayout->addWidget(buttonBox);

  detailsDialog->exec();
}

void ComplianceDashboard::updateCards() {
  if (!scrollArea) {
    qDebug() << "Error: scrollArea is null";
    return;
  }

  // Block signals during update to prevent recursive calls
  QSignalBlocker blocker(scrollArea);

  // Clear existing cards safely
  QWidget* oldContent = scrollArea->takeWidget();
  if (oldContent) {
    oldContent->deleteLater();
  }

  // Create new content
  QWidget* scrollContent = new QWidget();
  cardsLayout = new QGridLayout(scrollContent);
  cardsLayout->setSpacing(12);
  cardsLayout->setContentsMargins(12, 12, 12, 12);

  // Safety check for model
  if (model.rowCount(QModelIndex()) <= 0) {
    qDebug() << "Warning: No data in model";
    scrollArea->setWidget(scrollContent);
    return;
  }

  const int MAX_CARDS = 100;
  int cardCount = 0;
  int row = 0;
  int col = 0;

  for (int i = 0; i < model.rowCount(QModelIndex()) && cardCount < MAX_CARDS; ++i) {
    QString location = model.data(model.index(i, 2), Qt::DisplayRole).toString();
    QString pollutant = model.data(model.index(i, 4), Qt::DisplayRole).toString();
    QString result = model.data(model.index(i, 7), Qt::DisplayRole).toString();

    // Skip invalid data
    if (location.isEmpty() || pollutant.isEmpty() || result.isEmpty()) {
      continue;
    }

    // Apply filters
    if (!currentLocation.isEmpty() && currentLocation != "All Locations" && location != currentLocation)
      continue;

    if (!currentSearchText.isEmpty() && !pollutant.contains(currentSearchText, Qt::CaseInsensitive))
      continue;

    double value = result.toDouble();
    bool compliant = value <= getThresholdForPollutant(pollutant);

    if (!currentCompliance.isEmpty() && currentCompliance != "All") {
      if (currentCompliance == "Compliant" && !compliant)
        continue;
      if (currentCompliance == "Non-Compliant" && compliant)
        continue;
    }

    // Create frame for card
    QFrame* card = new QFrame();
    card->setObjectName("complianceCard");

    // Create and populate card
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    populateCardContent(cardLayout, pollutant, location, result, compliant);

    // Add to grid
    cardsLayout->addWidget(card, row, col);

    // Update position
    col++;
    if (col >= 3) {  // Move to next row after 3 cards
      col = 0;
      row++;
    }

    cardCount++;
  }

  if (cardCount >= MAX_CARDS) {
    QLabel* warningLabel = new QLabel("Showing first 100 results. Please refine your search.");
    warningLabel->setStyleSheet("color: #e74c3c;");
    cardsLayout->addWidget(warningLabel, row + 1, 0, 1, 3);  // Span all columns
  }

  scrollArea->setWidget(scrollContent);
}

void ComplianceDashboard::onLocationFiltered(const QString& location) {
  currentLocation = location;
  updateCards();
}

void ComplianceDashboard::onPollutantSearched(const QString& searchText) {
  currentSearchText = searchText;
  updateCards();
}

void ComplianceDashboard::onComplianceFiltered(const QString& compliance) {
  currentCompliance = compliance;
  updateCards();
}