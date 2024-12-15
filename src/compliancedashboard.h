#pragma once

#include "basepage.h"
#include "model.h"

class QGridLayout;
class QFrame;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;

class ComplianceDashboard : public BasePage {
  Q_OBJECT

 public:
  explicit ComplianceDashboard(QWidget* parent = nullptr);

 private slots:
  void onLocationFiltered(const QString& location);
  void onPollutantFiltered(const QString& pollutant);
  void onComplianceFiltered(const QString& compliance);

 private:
  QVBoxLayout* pageLayout;
  QHBoxLayout* filtersLayout;
  QFrame* filtersFrame;
  QGridLayout* cardsLayout;
  QFrame* cardsFrame;
  QLabel* locationLabel;
  QComboBox* locationSelect;
  QLabel* pollutantLabel;
  QComboBox* pollutantSelect;
  QLabel* complianceLabel;
  QComboBox* complianceSelect;

  int cardsRow = 0;
  int cardsCol = 0;
  const int cardsMaxCol = 3;

  SampleModel model;
  QStringList locations;
  QStringList pollutants;
  QString currentLocation;
  QString currentPollutant;
  QString currentCompliance;

  void setupUI() override;
  void setUpFilters(QHBoxLayout* layout);
  void setUpAndPopulateCards(QGridLayout* layout);
  void createComplianceCard(QGridLayout* layout, const QString& cardLocation,
                                       const QString& cardPollutant,
                                       const QString& cardResult,
                                       const QString& cardCompliance);
  void populateFilters();
};