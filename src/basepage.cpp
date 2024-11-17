#include "basepage.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

BasePage::BasePage(const QString& title, QWidget* parent)
    : QWidget(parent), pageTitle(title) {
  setupUI();
}

void BasePage::setupUI() {
  // Create main vertical layout
  mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // Setup the title section
  createHeader();

  // Create content area
  contentArea = new QWidget();
  contentArea->setObjectName("contentArea");
  contentArea->setStyleSheet(R"(
    QWidget#contentArea {
      border: 1px solid red;
    }
  )");
  contentArea->setMinimumSize(500, 720);
  mainLayout->addWidget(contentArea);
}

void BasePage::createHeader() {
  QWidget* headerContainer = new QWidget();
  headerContainer->setObjectName("headerContainer");
  headerContainer->setStyleSheet(R"(
    QWidget#headerContainer {
      border: 1px solid blue;
    }
  )");

  // Create layout for title container
  QHBoxLayout* headerLayout = new QHBoxLayout();
  headerContainer->setLayout(headerLayout);

  // Create and style the title label
  titleLabel = new QLabel(pageTitle);
  titleLabel->setStyleSheet(R"(
    color: white;
    font-size: 24px;  
    font-weight: bold;
  )");
  headerLayout->addWidget(titleLabel);

  // Add title container to main layout
  mainLayout->addWidget(headerContainer);
  mainLayout->addStretch();
}