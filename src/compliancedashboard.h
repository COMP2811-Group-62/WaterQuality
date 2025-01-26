#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QMap>
#include <QScrollArea>

#include "basepage.h"
#include "compliance.h"
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
  ComplianceDashboard(SampleModel* model, QWidget* parent = nullptr);
  void refreshView() override;

 protected:
  void showEvent(QShowEvent* event) override;

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
  SampleModel* model;
  QStringList locations;
  QStringList pollutants;
  QString currentLocation;
  QString currentSearchText;
  QString currentCompliance;

  // Add compliance status enum
  enum class ComplianceLevel {
    Compliant,
    Warning,
    Critical
  };

  void setupUI() override;
  void setUpFilters(QHBoxLayout* layout);
  void setUpScrollableCards();
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

  void resizeEvent(QResizeEvent* event) override;
  void adjustCardSizes();
  const int CARD_MARGIN = 12;
  const int MIN_CARD_WIDTH = 300;
};