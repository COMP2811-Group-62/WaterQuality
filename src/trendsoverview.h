#include "basepage.h"

class QHBoxLayout;

class TrendsOverviewPage : public BasePage {
 public:
  TrendsOverviewPage(QWidget* parent = nullptr);

 private:
  void setupUI() override;

  QHBoxLayout* pageLayout;
};