#include "compliancedashboard.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QScrollBar>
#include <QSet>
#include <QShowEvent>
#include <QString>
#include <QVBoxLayout>

#include "styles.h"

ComplianceDashboard::ComplianceDashboard(SampleModel* model, QWidget* parent)
    : BasePage(tr("Compliance Dashboard"), parent), model(model) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/compliancedashboard.qss"}));
  setupUI();
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

void ComplianceDashboard::refreshView() {
  populateFilters();  // Repopulate filters with new data
  updateCards();
}

void ComplianceDashboard::setUpFilters(QHBoxLayout* layout) {
  // Search Bar
  searchBar = new QLineEdit();
  searchBar->setPlaceholderText(tr("Search pollutants..."));
  searchBar->setObjectName("searchBar");

  locationLabel = new QLabel(tr("Location:"));
  locationSelect = new QComboBox();
  locationSelect->setObjectName("locationSelect");

  complianceLabel = new QLabel(tr("Compliance Status:"));
  complianceSelect = new QComboBox();
  complianceSelect->setObjectName("complianceSelect");

  complianceSelect->addItem(tr("All"));
  complianceSelect->addItem(tr("Compliant"));
  complianceSelect->addItem(tr("Non-Compliant"));

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

  for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {
    QString location = model->data(model->index(row, 2), Qt::DisplayRole).toString();
    QString pollutant = model->data(model->index(row, 4), Qt::DisplayRole).toString();

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

double ComplianceDashboard::getThresholdForPollutant(const QString& pollutant) {
  return Compliance::instance().getCriticalThreshold(pollutant);
}

QString ComplianceDashboard::getComplianceStatus(const QString& pollutant, double value) {
  auto& compliance = Compliance::instance();

  if (value <= compliance.getWarningThreshold(pollutant)) {
    return tr("Compliant");
  } else if (value <= compliance.getCriticalThreshold(pollutant)) {
    return tr("Warning");
  } else {
    return tr("Non-Compliant");
  }
}

QString ComplianceDashboard::getTrendAnalysis(const QString& pollutant, const QString& location) {
  return tr("Historical Analysis for %1 at %2:\n\n"
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
  // Create container frame and set styling
  QFrame* card = qobject_cast<QFrame*>(cardLayout->parentWidget());
  if (card) {
    // Set the status property based on compliance level
    double numValue = value.toDouble();
    auto& compliance = Compliance::instance();

    if (numValue <= compliance.getWarningThreshold(pollutant)) {
      card->setProperty("status", "compliant");
    } else if (numValue <= compliance.getCriticalThreshold(pollutant)) {
      card->setProperty("status", "warning");
    } else {
      card->setProperty("status", "critical");
    }

    // Force style update
    card->style()->unpolish(card);
    card->style()->polish(card);
  }

  // Rest of existing content population
  QLabel* nameLabel = new QLabel(pollutant);
  nameLabel->setObjectName("pollutantName");

  QLabel* locationLabel = new QLabel(QString("Location: %1").arg(location));
  locationLabel->setObjectName("locationLabel");

  QLabel* valueLabel = new QLabel(QString("Current Value: %1").arg(value));
  valueLabel->setObjectName("valueLabel");

  // Compliance Indicator
  QString complianceText;
  QString complianceStyle;

  double numValue = value.toDouble();
  auto& compliance = Compliance::instance();

  if (numValue <= compliance.getWarningThreshold(pollutant)) {
    complianceText = "✓ Compliant";
    complianceStyle = "color: #2ecc71; font-weight: bold;";  // Green
  } else if (numValue <= compliance.getCriticalThreshold(pollutant)) {
    complianceText = "✓ Compliant but Approaching Limit";
    complianceStyle = "color: #f1c40f; font-weight: bold;";  // Yellow
  } else {
    complianceText = "⚠ Non-Compliant";
    complianceStyle = "color: #e74c3c; font-weight: bold;";  // Red
  }

  QLabel* complianceLabel = new QLabel(complianceText);
  complianceLabel->setStyleSheet(complianceStyle);
  complianceLabel->setObjectName("complianceLabel");

  // Threshold Display
  double threshold = getThresholdForPollutant(pollutant);
  QLabel* thresholdLabel = new QLabel(QString("Threshold: %1").arg(threshold));
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
  detailsDialog->setWindowTitle(tr("%1 - Detailed Analysis").arg(pollutant));
  detailsDialog->setMinimumWidth(400);

  QVBoxLayout* dialogLayout = new QVBoxLayout(detailsDialog);

  // Current Status
  QLabel* statusLabel = new QLabel(tr("Current Status"));
  statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
  dialogLayout->addWidget(statusLabel);

  double threshold = getThresholdForPollutant(pollutant);
  bool isCompliant = value.toDouble() <= threshold;

  QString complianceInfo = isCompliant ? tr("✓ Currently within safe limits.") : tr("⚠ Exceeds safety threshold. Immediate attention required.");

  QLabel* complianceLabel = new QLabel(complianceInfo);
  complianceLabel->setStyleSheet(isCompliant ? "color: #2ecc71;" : "color: #e74c3c;");
  complianceLabel->setWordWrap(true);
  dialogLayout->addWidget(complianceLabel);

  // Trend Analysis
  QLabel* trendTitle = new QLabel(tr("Trend Analysis"));
  trendTitle->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
  dialogLayout->addWidget(trendTitle);

  QLabel* trendLabel = new QLabel(getTrendAnalysis(pollutant, location));
  trendLabel->setWordWrap(true);
  dialogLayout->addWidget(trendLabel);

  // Recommendations
  QLabel* recomTitle = new QLabel(tr("Recommendations"));
  recomTitle->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
  dialogLayout->addWidget(recomTitle);

  QString recommendations = isCompliant ? tr("• Continue regular monitoring\n"
                                             "• Maintain current control measures\n"
                                             "• Schedule next review in 30 days")
                                        : tr("• Increase monitoring frequency\n"
                                             "• Review control measures\n"
                                             "• Prepare incident report\n"
                                             "• Schedule immediate follow-up testing");

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
  cardsLayout->setSpacing(CARD_MARGIN);
  cardsLayout->setContentsMargins(CARD_MARGIN, CARD_MARGIN, CARD_MARGIN, CARD_MARGIN);

  // Safety check for model
  if (model->rowCount(QModelIndex()) <= 0) {
    qDebug() << "Warning: No data in model";
    scrollArea->setWidget(scrollContent);
    return;
  }

  const int MAX_CARDS = 100;
  int cardCount = 0;
  int row = 0;
  int col = 0;

  for (int i = 0; i < model->rowCount(QModelIndex()) && cardCount < MAX_CARDS; ++i) {
    QString location = model->data(model->index(i, 2), Qt::DisplayRole).toString();
    QString pollutant = model->data(model->index(i, 4), Qt::DisplayRole).toString();
    QString result = model->data(model->index(i, 7), Qt::DisplayRole).toString();

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
  adjustCardSizes();  // Adjust sizes after creating cards
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

void ComplianceDashboard::resizeEvent(QResizeEvent* event) {
  BasePage::resizeEvent(event);
  adjustCardSizes();
}

void ComplianceDashboard::adjustCardSizes() {
  if (!scrollArea || !cardsLayout)
    return;

  // Get available width accounting for scrollbar and margins
  int availableWidth = scrollArea->width() -
                       scrollArea->verticalScrollBar()->width() -
                       (CARD_MARGIN * 2);

  // Calculate number of columns that can fit
  int numColumns = std::max(1, availableWidth / (MIN_CARD_WIDTH + CARD_MARGIN));

  // Calculate optimal card width
  int cardWidth = (availableWidth - (CARD_MARGIN * (numColumns - 1))) / numColumns;

  // Update all cards in the grid
  for (int i = 0; i < cardsLayout->count(); ++i) {
    if (QWidget* card = cardsLayout->itemAt(i)->widget()) {
      card->setFixedWidth(cardWidth);
    }
  }

  // Update layout properties
  cardsLayout->setHorizontalSpacing(CARD_MARGIN);
  cardsLayout->setVerticalSpacing(CARD_MARGIN);
}

void ComplianceDashboard::showEvent(QShowEvent* event) {
  BasePage::showEvent(event);
  updateCards();
}