#include "mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

#include "dashboardpage.h"
#include "examplepage.h"
#include "navigationbar.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUI();
}

void MainWindow::setupUI() {
  // Set window properties
  resize(1280, 720);

#ifdef Q_OS_MAC
  setUnifiedTitleAndToolBarOnMac(true);
#endif

  // Create central widget
  centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Create main layout
  mainLayout = new QHBoxLayout(centralWidget);

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

  ExamplePage *examplePage = new ExamplePage();
  stackedWidget->addWidget(examplePage);
}

void MainWindow::switchPage(int index) {
  stackedWidget->setCurrentIndex(index);
}

void MainWindow::updateLanguage(int index) {
  // TODO: Implement language switching functionality
}