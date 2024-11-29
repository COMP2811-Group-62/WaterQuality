#include "datapage.h"

#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QTableView>

<<<<<<< HEAD
DataPage::DataPage(QWidget *parent)
    : BasePage("Data Page", parent) {
=======
#include "styles.h"

DataPage::DataPage(QWidget *parent)
    : BasePage("Data Page", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/datapage.qss"}));
>>>>>>> dev
  setupUI();
}

void DataPage::setupUI() {
  model.updateFromFile("../dataset/Y-2024-M.csv");

  table = new QTableView();
  table->setModel(&model);
  table->resizeColumnsToContents();

  QFont tableFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  table->setFont(tableFont);

  pageLayout = new QHBoxLayout(contentArea);
  pageLayout->setContentsMargins(0, 0, 0, 0);

  pageLayout->addWidget(table);
}
