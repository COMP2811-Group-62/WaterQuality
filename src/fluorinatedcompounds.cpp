#include "fluorinatedcompounds.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QXYSeries>

#include "styles.h"

FluorinatedCompounds::FluorinatedCompounds(QWidget* parent)
    : BasePage("Fluorinated Compounds Page", parent) {
  setupUI();
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/fluorinatedcompounds.qss"}));
}

void FluorinatedCompounds::setupUI() {
  QVBoxLayout* fullPage = new QVBoxLayout();
  QHBoxLayout* header = new QHBoxLayout();
  model.updateFromFile("../dataset/Y-2024-M.csv");

  // define items in header HBox
  QLabel* exampleLabel = new QLabel("This is an example page for FluorinatedCompounds!");
  exampleLabel->setObjectName("h1");
  header->addWidget(exampleLabel);
  fullPage->addLayout(header);

  // recreating the table to try to understand data accesss from model
  table = new QTableView();
  table->setModel(&model);
  fullPage->addWidget(table);

  // mock data for grap - QLineSeries required from model
  QLineSeries* series = new QLineSeries();
  series->append(1, 1);
  series->append(2, 9);
  series->append(3, 6);
  series->append(4, 4);
  series->append(5, 6);
  series->append(6, 8);
  series->append(7, 3);
  series->append(8, 2);

  // configure chart
  chart = new QChart();
  chart->addSeries(series);
  chart->createDefaultAxes();
  chart->axes(Qt::Vertical).first()->setRange(0, 10);
  chart->axes(Qt::Horizontal).first()->setRange(0, 10);
  chart->setVisible(true);

  // configure chartview
  QChartView* chartview = new QChartView(chart);
  chartview->setVisible(true);
  fullPage->addWidget(chartview);

  contentArea->setLayout(fullPage);
}