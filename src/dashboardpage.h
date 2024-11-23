#include "basepage.h"

class QGridLayout;
class QFrame;

class DashboardPage : public BasePage {
  Q_OBJECT

 public:
  DashboardPage(QWidget* parent = nullptr);

 private:
  QGridLayout* cardsLayout;
  void setupUI() override;
  QFrame* createCard(const QString& title, const QString& content);
};