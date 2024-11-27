#include <QFrame>
#include <QList>

class QVBoxLayout;
class QPushButton;
class QLabel;
class QComboBox;
class QPaintEvent;

class NavigationBar : public QFrame {
  Q_OBJECT

 public:
  explicit NavigationBar(QWidget *parent = nullptr);

 signals:
  void pageChanged(int index);
  void languageChanged(int index);

 private:
  void setupUI();
  QPushButton *createNavButton(const QString &text, int index, bool isChecked = false);

  QVBoxLayout *layout;
  QLabel *titleLabel;
  QList<QPushButton *> navButtons;
  QComboBox *languageSelector;
};