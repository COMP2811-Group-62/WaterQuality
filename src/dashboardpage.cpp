#include "dashboardpage.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

DashboardPage::DashboardPage(QWidget* parent)
    : BasePage("Dashboard", parent) {
  setupUI();
}

void DashboardPage::setupUI() {
  // Create grid layout for the content area
  cardsLayout = new QGridLayout();
  cardsLayout->setSpacing(20);

  // Create cards with titles and content
  QFrame* card1 = createCard("Common Pollutants",
                             "Overview of common water pollutants and their levels");
  cardsLayout->addWidget(card1, 0, 0);

  QFrame* card2 = createCard("POPs",
                             "Status of Persistent Organic Pollutants");
  cardsLayout->addWidget(card2, 0, 1);

  QFrame* card3 = createCard("Litter Indicators",
                             "Environmental litter levels and trends");
  cardsLayout->addWidget(card3, 1, 0);

  QFrame* card4 = createCard("Fluorinated Compounds",
                             "PFAS and other fluorinated compound levels");
  cardsLayout->addWidget(card4, 1, 1);

  QFrame* card5 = createCard("Compliance Status",
                             "Overall compliance with safety standards");
  cardsLayout->addWidget(card5, 2, 0, 1, 2);  // Spans 2 columns

  contentArea->setLayout(cardsLayout);
}

QFrame* DashboardPage::createCard(const QString& title, const QString& content) {
  QFrame* card = new QFrame();
  card->setObjectName("card");
  card->setStyleSheet(
      "QFrame#card {"
      "   background-color: white;"
      "   border: 1px solid #e0e0e0;"
      "   border-radius: 8px;"
      "}"
      "QLabel[title=\"true\"] {"
      "   font-size: 16px;"
      "   font-weight: bold;"
      "   color: #333333;"
      "   margin-bottom: 8px;"
      "}"
      "QLabel[content=\"true\"] {"
      "   color: #666666;"
      "}");

  QVBoxLayout* cardLayout = new QVBoxLayout(card);
  cardLayout->setContentsMargins(20, 20, 20, 20);

  // Title label
  QLabel* titleLabel = new QLabel(title);
  titleLabel->setProperty("title", "true");
  cardLayout->addWidget(titleLabel);

  // Content label
  QLabel* contentLabel = new QLabel(content);
  contentLabel->setProperty("content", "true");
  contentLabel->setWordWrap(true);
  cardLayout->addWidget(contentLabel);

  // Add stretch to push content to top
  cardLayout->addStretch();

  return card;
}