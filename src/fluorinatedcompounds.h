#include <QtCharts>

#include "basepage.h"
#include "model.h"

class QHBoxLayout;
class QTableView;
class QChart;

class FluorinatedCompounds : public BasePage {
 public:
  FluorinatedCompounds(QWidget* parent = nullptr);

 private:
  void setupUI() override;
  void configureMap(QVBoxLayout *fullPage);

  QChart* chart;
  QTableView* table;
  SampleModel model;
  QHBoxLayout* pageLayout;
};