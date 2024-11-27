#include "dashboardpage.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "styles.h"

DashboardPage::DashboardPage(QWidget* parent)
    : BasePage("Dashboard", parent) {
  setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                            ":/styles/dashboard.qss"}));
  setupUI();
}

void DashboardPage::setupUI() {
  cardsLayout = new QGridLayout(contentArea);
  cardsLayout->setSpacing(24);
  cardsLayout->setContentsMargins(24, 24, 24, 24);

  // Create cards
  cardsLayout->addWidget(
      createCard("Common Pollutants",
                 "Overview of common water pollutants and their levels"),
      0, 0);

  cardsLayout->addWidget(
      createCard("POPs",
                 "Status of Persistent Organic Pollutants"),
      0, 1);

  cardsLayout->addWidget(
      createCard("Litter Indicators",
                 "Environmental litter levels and trends"),
      1, 0);

  cardsLayout->addWidget(
      createCard("Fluorinated Compounds",
                 "PFAS and other fluorinated compound levels"),
      1, 1);

  cardsLayout->addWidget(
      createCard("Compliance Status",
                 "Overall compliance with safety standards"),
      2, 0, 1, 2);  // Spans 2 columns
}

QFrame* DashboardPage::createCard(const QString& title, const QString& subtitle) {
  QFrame* card = new QFrame();
  card->setObjectName("dashboardCard");

  QVBoxLayout* layout = new QVBoxLayout(card);
  layout->setSpacing(8);
  layout->setContentsMargins(0, 0, 0, 0);

  // Header container
  QWidget* header = new QWidget;
  header->setObjectName("cardHeader");
  QVBoxLayout* headerLayout = new QVBoxLayout(header);
  headerLayout->setSpacing(4);
  headerLayout->setContentsMargins(20, 20, 20, 20);

  // Title
  QLabel* titleLabel = new QLabel(title);
  titleLabel->setObjectName("cardTitle");
  headerLayout->addWidget(titleLabel);

  // Subtitle
  QLabel* subtitleLabel = new QLabel(subtitle);
  subtitleLabel->setObjectName("cardSubtitle");
  subtitleLabel->setWordWrap(true);
  headerLayout->addWidget(subtitleLabel);

  layout->addWidget(header);

  // Content area
  QWidget* content = new QWidget;
  content->setObjectName("cardContent");
  QVBoxLayout* contentLayout = new QVBoxLayout(content);
  contentLayout->setContentsMargins(20, 20, 20, 20);

  // Add placeholder text (remove this later)
  QLabel* placeholder = new QLabel("Content area");
  placeholder->setObjectName("cardPlaceholder");
  contentLayout->addWidget(placeholder);

  contentLayout->addStretch();
  layout->addWidget(content);

  return card;
}