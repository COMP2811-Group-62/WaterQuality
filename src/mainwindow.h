#pragma once

#include <QMainWindow>

class QWidget;
class QHBoxLayout;
class QFrame;
class QStackedWidget;
class QComboBox;
class QPushButton;
class QString;

class NavigationBar;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);

 private slots:
  void switchPage(int index);
  void updateLanguage(int index);

 private:
  void setupUI();
  void setupNavigation();
  void setupPages();

  QWidget *centralWidget;
  QHBoxLayout *mainLayout;
  NavigationBar *navBar;
  QStackedWidget *stackedWidget;
  QComboBox *languageSelector;

  // Helper function to create navigation buttons
  QPushButton *createNavButton(const QString &text, int index);
};