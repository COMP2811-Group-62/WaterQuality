#include "examplepage.h"

#include <QHBoxLayout>
#include <QLabel>

ExamplePage::ExamplePage(QWidget* parent)
    : BasePage("Example Page1", parent) {
  setupUI();
}

void ExamplePage::setupUI() {
  QHBoxLayout* pageLayout = new QHBoxLayout();

  QLabel* exampleLabel = new QLabel("This is an example page!");
  pageLayout->addWidget(exampleLabel);

  contentArea->setLayout(pageLayout);
}