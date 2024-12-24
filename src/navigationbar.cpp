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
  titleLabel = new QLabel(tr("AquaWatch"), this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("font-size: 32px; font-family: 'SF Pro'; color: #00c2e5; font-weight: bold; padding: 10px;");
  layout->addWidget(titleLabel);

  // Add navigation buttons
  navButtons.append(createNavButton(tr("Dashboard"), 0, true));  // Default
  navButtons.append(createNavButton(tr("Data Page"), 1, false));
  navButtons.append(createNavButton(tr("Pollutants Overview"), 2, false));
  navButtons.append(createNavButton(tr("Fluorinated Compounds"), 3, false));
  navButtons.append(createNavButton(tr("POPs Page"), 4, false));
  navButtons.append(createNavButton(tr("Litter Indicators"), 5, false));
  navButtons.append(createNavButton(tr("Compliance Dashboard"), 6, false));

  for (auto button : navButtons) {
    layout->addWidget(button);
  }

  layout->addStretch();
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

void NavigationBar::setCurrentPage(int index) {
  // Uncheck all buttons
  for (auto button : navButtons) {
    button->setChecked(false);
  }

  // Check the button corresponding to the index
  if (index >= 0 && index < navButtons.size()) {
    navButtons[index]->setChecked(true);
    emit pageChanged(index);
  }
}