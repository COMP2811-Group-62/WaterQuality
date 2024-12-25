#include "fluorinatedcompounds.h"

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QQuickItem>
#include <QTableView>
#include <QVBoxLayout>
#include <QXYSeries>

#include "styles.h"

FluorinatedCompounds::FluorinatedCompounds(SampleModel* model, QWidget* parent)
    : BasePage("Fluorinated Compounds Page", parent), model(model) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/fluorinatedcompounds.qss"}));
  setupUI();
}

void FluorinatedCompounds::setupUI() {
  // set up page layout
  page = new QVBoxLayout();
  body = new QHBoxLayout();
  header = new QVBoxLayout();
  columnLeft = new QVBoxLayout();
  columnRight = new QVBoxLayout();

  configureHeader(header);
  configureMap(columnLeft);
  configureSidebar(columnRight);

  body->addLayout(columnLeft);
  body->addLayout(columnRight);

  page->addLayout(header);
  page->addLayout(body);
  contentArea->setLayout(page);
}

void FluorinatedCompounds::refreshView() {
  findPollutants();
  clearMap();
}

void FluorinatedCompounds::configureHeader(QVBoxLayout* header) {
  QFrame* headerFrame = new QFrame();
  headerFrame->setObjectName("headerFrame");
  headerInner = new QVBoxLayout(headerFrame);
  mapControls = new QHBoxLayout();
  headerLables = new QVBoxLayout();

  mapControls->setSpacing(20);

  QLabel* titleLabel = new QLabel("The map below shows all Polutants with the 'PF' Prefix and the sampling point location.");
  titleLabel->setObjectName("h1");

  // define combo boxes

  pollutantSelector = new QComboBox();
  pollutantSelector->setPlaceholderText("Select Pollutant");
  findPollutants();

  timeRangeSelector = new QComboBox();
  timeRangeSelector->setPlaceholderText("Select Time range");
  timeRangeSelector->addItems({"January 2024", "February 2024", "March 2024",
                               "April 2024", "May 2024", "June 2024",
                               "July 2024", "August 2024", "September 2024",
                               "Most Recent"});

  locationSelector = new QComboBox();
  locationSelector->setPlaceholderText("Select Location");

  connect(pollutantSelector, &QComboBox::currentTextChanged,
          this, &FluorinatedCompounds::addMapCirlces);
  
  connect(locationSelector, &QComboBox::currentTextChanged,
          this, &FluorinatedCompounds::onLocationChange);

  QLabel* locationLabel = new QLabel("Location:");
  locationLabel->setObjectName("h2");
  QLabel* pollutantLabel = new QLabel("Pollutant:");
  pollutantLabel->setObjectName("h2");
  QLabel* timeRangeLabel = new QLabel("Time Range:");
  timeRangeLabel->setObjectName("h2");

  mapControls->addWidget(pollutantLabel);
  mapControls->addWidget(pollutantSelector);
  mapControls->addWidget(timeRangeLabel);
  mapControls->addWidget(timeRangeSelector);
  mapControls->addWidget(locationLabel);
  mapControls->addWidget(locationSelector);
  mapControls->addStretch();

  headerLables->addWidget(titleLabel);

  headerInner->addLayout(headerLables);
  headerInner->addLayout(mapControls);

  header->addWidget(headerFrame);
}

void FluorinatedCompounds::configureMap(QVBoxLayout* column) {
  // configure QQuickWiget which can display a QML project
  mapView = new QQuickWidget();
  mapView->setSource(QUrl(QStringLiteral("../src/fluorinatedcompounds-mapdisplay.qml")));
  mapView->setResizeMode(QQuickWidget::SizeRootObjectToView);
  mapView->show();

  column->addWidget(mapView);
}

void FluorinatedCompounds::configureSidebar(QVBoxLayout* column) {
  QFrame* sidebarFrameHeader = new QFrame();
  sidebarFrameHeader->setObjectName("sidebarFrameHeader");
  sidebarFrameHeader->setFixedHeight(100);
  sidbarInnerHeader = new QVBoxLayout(sidebarFrameHeader);

  QFrame* sidebarFrameBody = new QFrame();
  sidebarFrameBody->setObjectName("sidebarFrameBody");
  sidbarInnerBody = new QVBoxLayout(sidebarFrameBody);

  QLabel* titleLabel = new QLabel("Fluorinated compounds, often used in industrial applications, can pose serious health risks when they contaminate water supplies.");
  titleLabel->setWordWrap(true);
  titleLabel->setObjectName("h1");

  sidbarInnerHeader->addWidget(titleLabel);

  QLabel* bodyTitleLabel = new QLabel("Compliance Infomation:");
  bodyTitleLabel->setWordWrap(true);
  bodyTitleLabel->setObjectName("h1dark");

  sidbarInnerBody->addWidget(bodyTitleLabel);
  sidbarInnerBody->addStretch(1);

  column->addWidget(sidebarFrameHeader);
  column->addWidget(sidebarFrameBody);
}

void FluorinatedCompounds::findPollutants() {
  for (int i = 0; i < model->rowCount(QModelIndex()); i++) {
    QString pollutant = model->data(model->index(i, 4), Qt::DisplayRole).toString();
    
    if (pollutant.startsWith("PF", Qt::CaseInsensitive) ) {
      
      QString units = model->data(model->index(i, 8), Qt::DisplayRole).toString();
      QString result = model->data(model->index(i, 7), Qt::DisplayRole).toString();
      QString dateTime = model->data(model->index(i, 3), Qt::DisplayRole).toString();
      QString location = model->data(model->index(i, 2), Qt::DisplayRole).toString();
      QString dataURL = model->data(model->index(i, 1), Qt::DisplayRole).toString();

      dataPoint newPoint;
      newPoint.dataURL = dataURL;
      newPoint.location = location;
      newPoint.pollutant = pollutant;
      newPoint.date = dateTime;
      newPoint.result = result;
      newPoint.units = units;


      if (!filteredLocations.contains(location)) {
        filteredLocations.append(location);
      }
      if (!filteredPolutants.contains(pollutant)) {
        filteredPolutants.append(pollutant);
      }
      dataPoints.append(newPoint);

    }
  }

  if (!filteredLocations.empty() && !filteredPolutants.empty()) {
    pollutantSelector->addItems(filteredPolutants);
    locationSelector->addItems(filteredLocations);

  }
}

void FluorinatedCompounds::addMapCirlces() {

  clearMap();
  int counter = 0;
  QVariant colour = "green";
  QVariant dataURL = model->data(model->index(0, 1), Qt::DisplayRole).toString();

  QObject *rootObject = mapView->rootObject();

  for (int i = 0; i < dataPoints.size(); ++i) 
  { 
    dataPoint& point = dataPoints[i];  
    
    // add circle for each datapoint which matches the text in pollutant
    
    if (point.pollutant == pollutantSelector->currentText()) {
          QMetaObject::invokeMethod(rootObject, "addCircle", 
            Q_ARG(QVariant, colour),
            Q_ARG(QVariant, point.dataURL), 
            Q_ARG(QVariant, false),
            Q_ARG(QVariant, point.location),
            Q_ARG(QVariant, point.pollutant),
            Q_ARG(QVariant, point.date),
            Q_ARG(QVariant, point.result),
            Q_ARG(QVariant, point.units));

          counter++;
    }

  }
  qDebug() << counter << "Pollutant Circles added to the map for: " << pollutantSelector->currentText();
}

void FluorinatedCompounds::clearMap() {

  QObject *rootObject = mapView->rootObject();
  QMetaObject::invokeMethod(rootObject, "clearMap");

}

void FluorinatedCompounds::onLocationChange() {
  
  QVariant colour = "transparent";
  
  //find a point with the correct location name to pass to QML
  int i = 0;
  while (i < dataPoints.size()) 
  { 
    dataPoint& point = dataPoints[i];  
    
    if (point.location == locationSelector->currentText()) {

        QObject *rootObject = mapView->rootObject();
        QMetaObject::invokeMethod(rootObject, "addCircle", 
          Q_ARG(QVariant, colour),
          Q_ARG(QVariant, point.dataURL),
          Q_ARG(QVariant, true),
          Q_ARG(QVariant, point.location),
          Q_ARG(QVariant, point.pollutant),
          Q_ARG(QVariant, point.date),
          Q_ARG(QVariant, point.result),
          Q_ARG(QVariant, point.units));

        i = dataPoints.size();
    }
    i++;
  }



}