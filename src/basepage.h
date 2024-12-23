#pragma once

#include <QString>
#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;

class BasePage : public QWidget {
  Q_OBJECT

 public:
  explicit BasePage(const QString& title, QWidget* parent = nullptr);
  virtual ~BasePage() = default;
  virtual void loadDataset(const QString& filename) = 0;

 protected:
  // Content area where derived classes should add their widgets
  QWidget* contentArea;
  QVBoxLayout* mainLayout;

  // Setup functions that derived classes will use
  virtual void setupUI();
  virtual void createHeader();

 private:
  QString pageTitle;
  QLabel* titleLabel;
};