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
  page = new QVBoxLayout();
  header = new QVBoxLayout();
  model.updateFromFile("../dataset/Y-2024-M.csv");
  
  configureHeader(header);
  page->addLayout(header);
  configureMap(page);

  contentArea->setLayout(page);
}

void FluorinatedCompounds::configureHeader(QVBoxLayout *header) {

  QFrame* headerFrame = new QFrame();
  headerFrame->setObjectName("headerFrame");
  headerInner = new QVBoxLayout(headerFrame);
  mapControls = new QHBoxLayout();
  headerLables = new QVBoxLayout();
  mapControls->setSpacing(0);
  headerInner->setSpacing(20);
  mapControls->setSpacing(20);
  
  // define items in header HBox
  QLabel* titleLabel = new QLabel("Fluorinated compounds, often used in industrial applications, can pose serious health risks when they contaminate water supplies.");
  QLabel* subLabel = new QLabel("The map below shows all Polutants with the 'PF' Prefix and the sampling point location.");

  titleLabel->setObjectName("h2");
  subLabel->setObjectName("h1");

  locationSelector = new QComboBox();
  locationSelector->addItems({"Some", "Locations", "Will be", "Here"});

  pollutantSelector = new QComboBox();
  pollutantSelector->addItems({"Some", "PFAS", "Or any compound", "Starting with PF", "Will be", "Here"});

  timeRangeSelector = new QComboBox();
  timeRangeSelector->addItems({"Jan", "Feb", "March", "Or dynamic based off selections"});

  QLabel* locationLabel = new QLabel("Location:");
  locationLabel->setObjectName("h2");
  QLabel* pollutantLabel = new QLabel("Pollutant:");
  pollutantLabel->setObjectName("h2");
  QLabel* timeRangeLabel = new QLabel("Time Range:");
  timeRangeLabel->setObjectName("h2");

  mapControls->addWidget(locationLabel);
  mapControls->addWidget(locationSelector);
  mapControls->addWidget(pollutantLabel);
  mapControls->addWidget(pollutantSelector);
  mapControls->addWidget(timeRangeLabel);
  mapControls->addWidget(timeRangeSelector);
  mapControls->addStretch();
  
  headerLables->addWidget(subLabel);
  headerLables->addWidget(titleLabel);

  headerInner->addLayout(headerLables);
  headerInner->addLayout(mapControls);

  header->addWidget(headerFrame);

}

void FluorinatedCompounds::configureMap(QVBoxLayout *page) {

  //configure QQuickWiget which can display a QML project
  QQuickWidget *mapView = new QQuickWidget();
  mapView->setSource(QUrl(QStringLiteral("../src/fluorinatedcompounds-mapdisplay.qml")));
  mapView->setResizeMode(QQuickWidget::SizeRootObjectToView);
  mapView->setFocusPolicy(Qt::StrongFocus);
  mapView->show();

  page->addWidget(mapView);

}

