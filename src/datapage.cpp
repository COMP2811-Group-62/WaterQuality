#include "datapage.h"

#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QTableView>

#include "styles.h"

DataPage::DataPage(SampleModel* model, QWidget* parent)
    : BasePage("Data Page", parent), model(model) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/datapage.qss"}));
  setupUI();
}

void DataPage::setupUI() {
  table = new QTableView();
  table->setModel(model);
  table->resizeColumnsToContents();

  // hide columns
  table->setColumnHidden(0, true);  // ID
  table->setColumnHidden(1, true);  // Sampling Point Notation

  QFont tableFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  table->setFont(tableFont);

  pageLayout = new QHBoxLayout(contentArea);
  pageLayout->setContentsMargins(0, 0, 0, 0);

  pageLayout->addWidget(table);
}

void DataPage::refreshView() {
  if (table) {
    table->reset();
    table->resizeColumnsToContents();
  }
}
