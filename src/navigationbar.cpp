#include "navigationbar.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "styles.h"

NavigationBar::NavigationBar(QWidget *parent) : QFrame(parent) {
  setObjectName("navigationBar");
  setStyleSheet(Styles::loadStyleSheet(":/styles/navigation.qss"));
  setupUI();
}

void NavigationBar::setupUI() {
  layout = new QVBoxLayout(this);

  // Add title
  titleLabel = new QLabel("AquaWatch", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("font-size: 32px; font-family: 'SF Pro'; color: #00c2e5; font-weight: bold; padding: 10px;");
  layout->addWidget(titleLabel);

  // Add navigation buttons
  navButtons.append(createNavButton("Dashboard", 0, true));  // Default
  navButtons.append(createNavButton("Data Page", 1, false));
  navButtons.append(createNavButton("Pollutant Trends", 2, false));
  navButtons.append(createNavButton("Example Page", 3, false));
  navButtons.append(createNavButton("Fluorinated Compounds", 4, false));
  navButtons.append(createNavButton("POPs Page", 5, false));
  navButtons.append(createNavButton("Litter Indicators", 6, false));

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