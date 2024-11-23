// src/litterpage.h  (确认此文件存在并包含以下内容)
#pragma once

#include "basepage.h"

class QHBoxLayout;
class QComboBox;

class LitterPage : public BasePage {
    Q_OBJECT

public:
    LitterPage(QWidget* parent = nullptr);

private:
    void setupUI() override;
    QHBoxLayout* pageLayout;
    QComboBox* locationFilter;
    QComboBox* litterTypeFilter;
};