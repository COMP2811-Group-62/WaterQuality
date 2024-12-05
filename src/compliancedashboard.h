#include "basepage.h"

class QHBoxLayout;

class ComplianceDashboard : public BasePage {
 public:
  ComplianceDashboard(QWidget* parent = nullptr);

 private:
  void setupUI() override;

  QHBoxLayout* pageLayout;
};