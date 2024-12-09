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
  void configureMap(QVBoxLayout *column);
  void configureSidebar(QVBoxLayout *column);

  SampleModel model;

  QVBoxLayout* header;
  QHBoxLayout* body;
  QVBoxLayout* page;
  QVBoxLayout* columnLeft;
  QVBoxLayout* columnRight;

  QVBoxLayout* headerInner;
  QHBoxLayout* mapControls;
  QVBoxLayout* headerLables;

  QComboBox* locationSelector;
  QComboBox* pollutantSelector;
  QComboBox* timeRangeSelector;

};