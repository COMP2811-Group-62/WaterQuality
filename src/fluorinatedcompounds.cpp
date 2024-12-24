#include "fluorinatedcompounds.h"

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QXYSeries>

#include "styles.h"

FluorinatedCompounds::FluorinatedCompounds(SampleModel* model, QWidget* parent)
    : BasePage(tr("Fluorinated Compounds Page"), parent), model(model) {
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
  // Implement logic to refresh the view based on the shared model
  // For example, update charts or tables
}

void FluorinatedCompounds::configureHeader(QVBoxLayout* header) {
  QFrame* headerFrame = new QFrame();
  headerFrame->setObjectName("headerFrame");
  headerInner = new QVBoxLayout(headerFrame);
  mapControls = new QHBoxLayout();
  headerLables = new QVBoxLayout();

  mapControls->setSpacing(20);

  QLabel* titleLabel = new QLabel(tr("The map below shows all Polutants with the 'PF' Prefix and the sampling point location."));
  titleLabel->setObjectName("h1");

  // define combo boxes
  locationSelector = new QComboBox();
  locationSelector->addItems({"Some", "Locations", "Will be", "Here"});

  pollutantSelector = new QComboBox();
  pollutantSelector->addItems({"Some", "PFAS", "Or any compound", "Starting with PF", "Will be", "Here"});

  timeRangeSelector = new QComboBox();
  timeRangeSelector->addItems({"Jan", "Feb", "March", "Or dynamic based off selections"});

  QLabel* locationLabel = new QLabel(tr("Location:"));
  locationLabel->setObjectName("h2");
  QLabel* pollutantLabel = new QLabel(tr("Pollutant:"));
  pollutantLabel->setObjectName("h2");
  QLabel* timeRangeLabel = new QLabel(tr("Time Range:"));
  timeRangeLabel->setObjectName("h2");

  mapControls->addWidget(locationLabel);
  mapControls->addWidget(locationSelector);
  mapControls->addWidget(pollutantLabel);
  mapControls->addWidget(pollutantSelector);
  mapControls->addWidget(timeRangeLabel);
  mapControls->addWidget(timeRangeSelector);
  mapControls->addStretch();

  headerLables->addWidget(titleLabel);

  headerInner->addLayout(headerLables);
  headerInner->addLayout(mapControls);

  header->addWidget(headerFrame);
}

void FluorinatedCompounds::configureMap(QVBoxLayout* column) {
  // configure QQuickWiget which can display a QML project
  QQuickWidget* mapView = new QQuickWidget();
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
