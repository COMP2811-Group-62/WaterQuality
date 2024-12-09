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
  void configureHeader(QVBoxLayout *header);
  void configureMap(QVBoxLayout *page);

  SampleModel model;

  QVBoxLayout* header;
  QVBoxLayout* page;

  QVBoxLayout* headerInner;
  QHBoxLayout* mapControls;
  QVBoxLayout* headerLables;

  QComboBox* locationSelector;
  QComboBox* pollutantSelector;
  QComboBox* timeRangeSelector;

};