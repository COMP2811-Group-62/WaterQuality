#include "trendsoverview.h"

#include <QHBoxLayout>
#include <QLabel>

TrendsOverviewPage::TrendsOverviewPage(QWidget* parent)
    : BasePage("Pollutant Trends", parent) {
  setupUI();
}

void TrendsOverviewPage::setupUI() {
  QHBoxLayout* pageLayout = new QHBoxLayout();

  QLabel* exampleLabel = new QLabel("This is an example page!");
  pageLayout->addWidget(exampleLabel);

  contentArea->setLayout(pageLayout);
}