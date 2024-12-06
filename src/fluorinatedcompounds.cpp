#include "fluorinatedcompounds.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
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

  //set up page layout
  QVBoxLayout* fullPage = new QVBoxLayout();
  QVBoxLayout* header = new QVBoxLayout();
  model.updateFromFile("../dataset/Y-2024-M.csv");

  // define items in header HBox
  QLabel* titleLabel = new QLabel("Fluorinated compounds, often used in industrial applications, can pose serious health risks when they contaminate water supplies.");
  QLabel* subLabel = new QLabel("The map below shows all Polutants with the 'PF' Prefix and the sampling point location.");

  titleLabel->setObjectName("h1");
  subLabel->setObjectName("h1");

  header->addWidget(titleLabel);
  header->addWidget(subLabel);

  fullPage->addLayout(header);

  configureMap(fullPage);

  
  // recreating the table to try to understand data accesss from model
  table = new QTableView();
  table->setModel(&model);
  //fullPage->addWidget(table);

  // // mock data for grap - QLineSeries required from model
  // QLineSeries* series = new QLineSeries();
  // series->append(1, 1);
  // series->append(2, 9);
  // series->append(3, 6);
  // series->append(4, 4);
  // series->append(5, 6);
  // series->append(6, 8);
  // series->append(7, 3);
  // series->append(8, 2);

  // // configure chart
  // chart = new QChart();
  // chart->addSeries(series);
  // chart->createDefaultAxes();
  // chart->axes(Qt::Vertical).first()->setRange(0, 10);
  // chart->axes(Qt::Horizontal).first()->setRange(0, 10);
  // chart->setVisible(true);

  // // configure chartview
  // QChartView* chartview = new QChartView(chart);
  // chartview->setVisible(true);
  // fullPage->addWidget(chartview);

   

  contentArea->setLayout(fullPage);
}

void FluorinatedCompounds::configureMap(QVBoxLayout *fullPage) {

  //configure QQuickWiget which can display a QML project
  QQuickWidget *mapView = new QQuickWidget();
  mapView->setSource(QUrl(QStringLiteral("../src/fluorinatedcompounds-mapdisplay.qml")));
  mapView->setResizeMode(QQuickWidget::SizeRootObjectToView);
  mapView->setFocusPolicy(Qt::StrongFocus);
  mapView->show();

  fullPage->addWidget(mapView);

}