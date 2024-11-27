#include "fluorinatedcompounds.h"

#include <QHBoxLayout>
#include <QLabel>

#include "styles.h"

FluorinatedCompounds::FluorinatedCompounds(QWidget* parent)
    : BasePage("Fluorinated Compounds Page", parent) {
  setupUI();
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/fluorinatedcompounds.qss"}));
}

void FluorinatedCompounds::setupUI() {
  QHBoxLayout* pageLayout = new QHBoxLayout();

  QLabel* exampleLabel = new QLabel("This is an example page for FluorinatedCompounds!");
  exampleLabel->setObjectName("h1");
  pageLayout->addWidget(exampleLabel);

  contentArea->setLayout(pageLayout);
}