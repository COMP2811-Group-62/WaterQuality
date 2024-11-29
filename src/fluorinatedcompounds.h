<<<<<<< HEAD
#include "basepage.h"

class QHBoxLayout;

class FluorinatedCompounds : public BasePage {

=======
#include <QtCharts>

#include "basepage.h"
#include "model.h"

class QHBoxLayout;
class QTableView;
class QChart;

class FluorinatedCompounds : public BasePage {
>>>>>>> dev
 public:
  FluorinatedCompounds(QWidget* parent = nullptr);

 private:
  void setupUI() override;

<<<<<<< HEAD
=======
  QChart* chart;
  QTableView* table;
  SampleModel model;
>>>>>>> dev
  QHBoxLayout* pageLayout;
};