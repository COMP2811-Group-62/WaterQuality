// compliancedashboard.h
#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QMap>
#include <QScrollArea>

#include "basepage.h"
#include "model.h"

class QGridLayout;
class QFrame;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QPushButton;

class ComplianceDashboard : public BasePage {
  Q_OBJECT

 public:
  explicit ComplianceDashboard(QWidget* parent = nullptr);
  void loadDataset(const QString& filename) override;

 private slots:
  void onLocationFiltered(const QString& location);
  void onPollutantSearched(const QString& searchText);
  void onComplianceFiltered(const QString& compliance);
  void showPollutantDetails(const QString& pollutant,
                            const QString& location,
                            const QString& value);

 private:
  // UI Components
  QVBoxLayout* pageLayout;
  QHBoxLayout* filtersLayout;
  QFrame* filtersFrame;
  QGridLayout* cardsLayout;
  QFrame* cardsFrame;
  QScrollArea* scrollArea;

  // Search and Filters
  QLineEdit* searchBar;
  QLabel* locationLabel;
  QComboBox* locationSelect;
  QLabel* complianceLabel;
  QComboBox* complianceSelect;

  // Data Management
  SampleModel model;
  QStringList locations;
  QStringList pollutants;
  QString currentLocation;
  QString currentSearchText;
  QString currentCompliance;

  // Compliance Thresholds (µg/L)
  const QMap<QString, double> COMPLIANCE_THRESHOLDS = {
      {"PFAS", 0.1},
      {"1,1,2-Trichloroethane", 0.2},
      {"Chloroform", 0.3},
      {"PCBs", 0.05}};

  void setupUI() override;
  void setUpFilters(QHBoxLayout* layout);
  void setUpScrollableCards();
  void createComplianceCard(QVBoxLayout* layout,
                            const QString& pollutant,
                            const QString& location,
                            const QString& value,
                            bool compliant);
  void populateFilters();
  void showTrendPopup(const QString& pollutant, const QString& location);
  double getThresholdForPollutant(const QString& pollutant);
  QString getComplianceStatus(const QString& pollutant, double value);
  QString getTrendAnalysis(const QString& pollutant, const QString& location);
  void updateCards();
  void populateCardContent(QVBoxLayout* cardLayout,
                           const QString& pollutant,
                           const QString& location,
                           const QString& value,
                           bool compliant);
};