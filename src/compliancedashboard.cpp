#include "compliancedashboard.h"

#include <QHBoxLayout>
#include <QLabel>

ComplianceDashboard::ComplianceDashboard(QWidget* parent)
    : BasePage("Compliance Dashboard", parent) {
  setupUI();
}

void ComplianceDashboard::setupUI() {
  QHBoxLayout* pageLayout = new QHBoxLayout();

  QLabel* exampleLabel = new QLabel("This is the compliance dashboard page!");
  pageLayout->addWidget(exampleLabel);

  contentArea->setLayout(pageLayout);
}