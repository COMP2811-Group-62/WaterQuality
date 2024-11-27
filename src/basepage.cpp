#include "basepage.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

#include "styles.h"

BasePage::BasePage(const QString& title, QWidget* parent)
    : QWidget(parent), pageTitle(title) {
  setObjectName("basePage");
  setStyleSheet(Styles::loadStyleSheet(":/styles/basepage.qss"));
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

  mainLayout->addWidget(contentArea, 1);
}

void BasePage::createHeader() {
  QWidget* headerContainer = new QWidget();
  headerContainer->setObjectName("headerContainer");
  headerContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  // Create layout for title container
  QHBoxLayout* headerLayout = new QHBoxLayout(headerContainer);
  headerLayout->setContentsMargins(24, 24, 24, 24);

  // Create and style the title label
  titleLabel = new QLabel(pageTitle);
  titleLabel->setObjectName("pageTitle");
  headerLayout->addWidget(titleLabel);

  // Add title container to main layout
  mainLayout->addWidget(headerContainer);
}