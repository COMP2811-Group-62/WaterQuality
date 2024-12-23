#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

#include "compliancedashboard.h"
#include "dashboardpage.h"
#include "datapage.h"
#include "fluorinatedcompounds.h"
#include "litterpage.h"
#include "navigationbar.h"
#include "popspage.h"
#include "trendsoverview.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setupUI();

  // Loads default dataset on startup, you can comment this out to test unloaded state.
  loadDataset("../dataset/Y-2024-M.csv");
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

  // Setup menu bar
  setupMenu();
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
  DashboardPage* dbPage = new DashboardPage();
  stackedWidget->addWidget(dbPage);

  connect(dbPage, &DashboardPage::pageChangeRequested,
          navBar, &NavigationBar::setCurrentPage);

  DataPage* dataPage = new DataPage();
  stackedWidget->addWidget(dataPage);

  TrendsOverviewPage* pollutantsOverview = new TrendsOverviewPage();
  stackedWidget->addWidget(pollutantsOverview);

  FluorinatedCompounds* fluorinatedCompounds = new FluorinatedCompounds();
  stackedWidget->addWidget(fluorinatedCompounds);

  POPsPage* popsPage = new POPsPage();
  stackedWidget->addWidget(popsPage);

  LitterPage* litterPage = new LitterPage();
  stackedWidget->addWidget(litterPage);

  ComplianceDashboard* complianceDashboard = new ComplianceDashboard();
  stackedWidget->addWidget(complianceDashboard);
}

void MainWindow::switchPage(int index) {
  stackedWidget->setCurrentIndex(index);
}

void MainWindow::setupMenu() {
  QMenuBar* menuBar = new QMenuBar(this);
  setMenuBar(menuBar);

  QMenu* fileMenu = new QMenu("&File", menuBar);
  menuBar->addMenu(fileMenu);

  QAction* openAction = new QAction("&Open Dataset...", this);
  openAction->setShortcut(QKeySequence::Open);
  fileMenu->addAction(openAction);

  connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
}

void MainWindow::openFile() {
  QString filename = QFileDialog::getOpenFileName(this,
                                                  "Load Dataset", "", "CSV Files (*.csv);;All Files (*)");

  if (!filename.isEmpty()) {
    loadDataset(filename);
  }
}

void MainWindow::loadDataset(const QString& filename) {
  try {
    // Verify file exists and is readable
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::critical(this, "Error", "Cannot open file: " + filename);
      return;
    }
    file.close();

    distributeDataset(filename);
    QMessageBox::information(this, "Success", "Dataset loaded successfully");

  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error",
                          QString("Error loading dataset: %1").arg(e.what()));
  }
}

void MainWindow::distributeDataset(const QString& filename) {
  // Update each page with the new dataset
  for (int i = 0; i < stackedWidget->count(); i++) {
    if (auto page = qobject_cast<BasePage*>(stackedWidget->widget(i))) {
      try {
        page->loadDataset(filename);
      } catch (const std::exception& e) {
        qWarning() << "Error updating page" << i << ":" << e.what();
      }
    }
  }

  // Update the current page's display
  if (auto currentPage = qobject_cast<BasePage*>(stackedWidget->currentWidget())) {
    currentPage->update();
  }
}

void MainWindow::updateLanguage(int index) {
  // TODO: Implement language switching functionality
}