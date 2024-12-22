#include "mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

#include "dashboardpage.h"
#include "datapage.h"
#include "fluorinatedcompounds.h"
#include "litterpage.h"
#include "navigationbar.h"
#include "popspage.h"
#include "trendsoverview.h"
#include "compliancedashboard.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUI();
}

void MainWindow::setupUI() {
  // Set window properties
  resize(1280, 720);

  // Create central widget
  centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Create main layout
  mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Create and setup navigation bar
  setupNavigation();

  // Create stacked widget for pages
  stackedWidget = new QStackedWidget;
  mainLayout->addWidget(stackedWidget);

  // Create all pages
  setupPages();
}

void MainWindow::setupNavigation() {
  navBar = new NavigationBar(this);
  mainLayout->addWidget(navBar);

  // Connect navigation signals
  connect(navBar, &NavigationBar::pageChanged,
          this, &MainWindow::switchPage);
  connect(navBar, &NavigationBar::languageChanged,
          this, &MainWindow::updateLanguage);
}

void MainWindow::setupPages() {
  DashboardPage *dbPage = new DashboardPage();
  stackedWidget->addWidget(dbPage);

  DataPage *dataPage = new DataPage();
  stackedWidget->addWidget(dataPage);

  TrendsOverviewPage *pollutantsOverview = new TrendsOverviewPage();
  stackedWidget->addWidget(pollutantsOverview);

  FluorinatedCompounds *fluorinatedCompounds = new FluorinatedCompounds();
  stackedWidget->addWidget(fluorinatedCompounds);

  POPsPage *popsPage = new POPsPage();
  stackedWidget->addWidget(popsPage);

  LitterPage *litterPage = new LitterPage();
  stackedWidget->addWidget(litterPage);

  ComplianceDashboard *complianceDashboard = new ComplianceDashboard();
  stackedWidget->addWidget(complianceDashboard);
}

void MainWindow::switchPage(int index) {
  stackedWidget->setCurrentIndex(index);
}

void MainWindow::updateLanguage(int index) {
  // TODO: Implement language switching functionality
}