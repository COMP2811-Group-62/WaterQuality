#include "basepage.h"

class QHBoxLayout;

class FluorinatedCompounds : public BasePage {

 public:
  FluorinatedCompounds(QWidget* parent = nullptr);

 private:
  void setupUI() override;

  QHBoxLayout* pageLayout;
};