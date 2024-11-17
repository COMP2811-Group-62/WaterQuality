#include "basepage.h"

class QHBoxLayout;

class ExamplePage : public BasePage {
 public:
  ExamplePage(QWidget* parent = nullptr);

 private:
  void setupUI() override;

  QHBoxLayout* pageLayout;
};