#include "fluorinatedcompounds.h"

#include <QHBoxLayout>
#include <QLabel>

FluorinatedCompounds::FluorinatedCompounds(QWidget* parent)
    : BasePage("Fluorinated Compounds Page", parent) {
  setupUI();
}

void FluorinatedCompounds::setupUI() {
  QHBoxLayout* pageLayout = new QHBoxLayout();

  QLabel* exampleLabel = new QLabel("This is an example page for FluorinatedCompounds!");
  pageLayout->addWidget(exampleLabel);

  contentArea->setLayout(pageLayout);
}