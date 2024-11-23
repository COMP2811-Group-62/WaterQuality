#include "navigationbar.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

NavigationBar::NavigationBar(QWidget *parent) : QFrame(parent) {
  setupUI();
}

void NavigationBar::setupUI() {
  setObjectName("navigationBar");
  setStyleSheet(R"(
        QFrame#navigationBar {
            border: 1px;
            border-right: 1px solid #037a9b;
        }
        QPushButton {
            text-align: left;
            padding: 4px 20px;
            border-radius: 5px;
            margin: 2px 8px;
            color: white;
        }
        QPushButton:checked {
            background-color: rgba(255, 255, 255, 0.1);
        }
        QPushButton:hover:!checked {
            background-color: rgba(0, 0, 0, 0.05);
        }
    )");

  setMaximumWidth(200);

  layout = new QVBoxLayout(this);

  // Add title
  titleLabel = new QLabel("AquaWatch", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("font-size: 16px; color: #00c2e5; font-weight: bold; padding: 10px;");
  layout->addWidget(titleLabel);

  // Add navigation buttons
  navButtons.append(createNavButton("Dashboard", 0, true));  // Default
  navButtons.append(createNavButton("ExamplePage", 1, false));
  navButtons.append(createNavButton("Litter", 2, false));

  for (auto button : navButtons) {
    layout->addWidget(button);
  }

  layout->addStretch();

  // Add language selector
  languageSelector = new QComboBox(this);
  languageSelector->addItems({"English", "Français", "Español"});
  connect(languageSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &NavigationBar::languageChanged);
  layout->addWidget(languageSelector);
}

QPushButton *NavigationBar::createNavButton(const QString &text, int index, bool isChecked) {
  QPushButton *button = new QPushButton(text, this);
  button->setCheckable(true);
  button->setAutoExclusive(true);
  if (isChecked) {
    button->setChecked(true);
  }
  connect(button, &QPushButton::clicked, [this, index]() {
    emit pageChanged(index);
  });
  return button;
}