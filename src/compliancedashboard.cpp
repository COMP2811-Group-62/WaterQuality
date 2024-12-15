#include "compliancedashboard.h"
#include "styles.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSet>
#include <QList>
#include <QString>

ComplianceDashboard::ComplianceDashboard(QWidget* parent)
    : BasePage("Compliance Dashboard", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/compliancedashboard.qss"}));
  setupUI();
  populateFilters();
}

void ComplianceDashboard::setupUI() {
  model.updateFromFile("../dataset/Y-2024-M.csv");
  pageLayout = new QVBoxLayout();
  filtersFrame = new QFrame();
  filtersFrame->setObjectName("filtersFrame");
  filtersLayout = new QHBoxLayout(filtersFrame);
  filtersLayout->setSpacing(24);
  setUpFilters(filtersLayout);
  pageLayout->addWidget(filtersFrame);
  cardsFrame = new QFrame();
  cardsFrame->setObjectName("cardsFrame");
  cardsLayout = new QGridLayout(cardsFrame);
  cardsLayout->setSpacing(24);
  cardsLayout->setContentsMargins(12, 12, 12, 12);
  setUpAndPopulateCards(cardsLayout);
  pageLayout->addWidget(cardsFrame);
  contentArea->setLayout(pageLayout);
}

void ComplianceDashboard::setUpFilters(QHBoxLayout* layout) {
  locationLabel = new QLabel("Select location:");
  locationSelect = new QComboBox();
  pollutantLabel = new QLabel("Select pollutant:");
  pollutantSelect = new QComboBox();
  complianceLabel = new QLabel("Select compliance status:");
  complianceSelect = new QComboBox();

  complianceSelect->addItem("Safe");
  complianceSelect->addItem("Hazardous");

  layout->addWidget(locationLabel);
  layout->addWidget(locationSelect);
  layout->addWidget(pollutantLabel);
  layout->addWidget(pollutantSelect);
  layout->addWidget(complianceLabel);
  layout->addWidget(complianceSelect);
  layout->addStretch();

  connect(locationSelect, &QComboBox::currentTextChanged, this, &ComplianceDashboard::onLocationFiltered);
  connect(pollutantSelect, &QComboBox::currentTextChanged, this, &ComplianceDashboard::onPollutantFiltered);
  connect(complianceSelect, &QComboBox::currentTextChanged, this, &ComplianceDashboard::onComplianceFiltered);
}

void ComplianceDashboard::populateFilters() {
  locationSelect->clear();
  pollutantSelect->clear();
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

  locationSelect->addItems(locations);
  pollutantSelect->addItems(pollutants);
}

void ComplianceDashboard::setUpAndPopulateCards(QGridLayout* layout) {

  QLayoutItem* child;
  while ((child = layout->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }
  
  for (int i = 0; i < model.rowCount(QModelIndex()); ++i) {
    QString location = model.data(model.index(i, 2), Qt::DisplayRole).toString();
    QString pollutant = model.data(model.index(i, 4), Qt::DisplayRole).toString();
    QString result = model.data(model.index(i, 7), Qt::DisplayRole).toString();
    QString compliance;
    
    bool isSafe = result.toDouble() <= 0.1;
    if (isSafe) {
      compliance = "Safe";
    }
    else {
      compliance = "Hazardous";
    }

    if (compliance != currentCompliance) {
        continue;
    }
    if (location != currentLocation) {
        continue;
    }
    if (pollutant != currentPollutant) {
        continue;
    }

    createComplianceCard(layout, location, pollutant, result, compliance);
}

}

void ComplianceDashboard::createComplianceCard(QGridLayout* layout, 
                                              const QString& cardLocation,
                                              const QString& cardPollutant,
                                              const QString& cardResult,
                                              const QString& cardCompliance) {

  QFrame* complianceCard = new QFrame();
  complianceCard->setObjectName("complianceCard");

  QVBoxLayout* complianceCardLayout = new QVBoxLayout(complianceCard);

  QLabel* locationCardLabel = new QLabel("Location: " + cardLocation);
  locationCardLabel->setObjectName("locationCardLabel");
  QLabel* pollutantCardLabel = new QLabel("Pollutant: " + cardPollutant);
  pollutantCardLabel->setObjectName("pollutantCardLabel");
  QLabel* resultCardLabel = new QLabel("Result: " + cardResult);
  resultCardLabel->setObjectName("resultCardLabel");
  QLabel* complianceCardLabel = new QLabel("Compliance: " + cardCompliance);
  complianceCardLabel->setObjectName("complianceCardLabel");

  if (cardCompliance == "Safe") {
    complianceCardLabel->setStyleSheet("color: green; font-weight: bold;");
  } else if (cardCompliance == "Hazardous") {
    complianceCardLabel->setStyleSheet("color: red; font-weight: bold;");
  }

  complianceCardLayout->addWidget(locationCardLabel);
  complianceCardLayout->addWidget(pollutantCardLabel);
  complianceCardLayout->addWidget(resultCardLabel);
  complianceCardLayout->addWidget(complianceCardLabel);
  complianceCardLayout->setSpacing(12);
  complianceCardLayout->setContentsMargins(12, 12, 12, 12);

  layout->addWidget(complianceCard, cardsRow, cardsCol);
  cardsCol++;
  if (cardsCol >= cardsMaxCol) {
    cardsCol = 0;
    cardsRow++;
  }
}

void ComplianceDashboard::onLocationFiltered(const QString& location) {
  currentLocation = location;
  setUpAndPopulateCards(cardsLayout);
}

void ComplianceDashboard::onPollutantFiltered(const QString& pollutant) {
  currentPollutant = pollutant;
  setUpAndPopulateCards(cardsLayout);
}

void ComplianceDashboard::onComplianceFiltered(const QString& compliance) {
  currentCompliance = compliance;
  setUpAndPopulateCards(cardsLayout);
}
