#include "litterpage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>

LitterPage::LitterPage(QWidget *parent)
    : BasePage("Litter Indicators", parent)
{
    setupUI();
}

void LitterPage::setupUI()
{
    // 创建水平布局
    pageLayout = new QHBoxLayout();

    // 创建左侧筛选器面板
    QVBoxLayout *filterLayout = new QVBoxLayout();

    // 添加位置筛选器
    QLabel *locationLabel = new QLabel("Location:", this);
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    locationFilter->addItem("Beach A");
    locationFilter->addItem("River B");
    locationFilter->addItem("Lake C");

    filterLayout->addWidget(locationLabel);
    filterLayout->addWidget(locationFilter);

    // 添加垃圾类型筛选器
    QLabel *typeLabel = new QLabel("Litter Type:", this);
    litterTypeFilter = new QComboBox(this);
    litterTypeFilter->addItem("All Types");
    litterTypeFilter->addItem("Plastics");
    litterTypeFilter->addItem("Debris");
    litterTypeFilter->addItem("Others");

    filterLayout->addWidget(typeLabel);
    filterLayout->addWidget(litterTypeFilter);

    // 添加弹性空间
    filterLayout->addStretch();

    // 将筛选器面板添加到主布局
    pageLayout->addLayout(filterLayout);

    // 添加一个占位标签（后续可替换为图表）
    QLabel *chartPlaceholder = new QLabel("Chart Area - Coming Soon", this);
    chartPlaceholder->setAlignment(Qt::AlignCenter);
    chartPlaceholder->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ddd;");
    pageLayout->addWidget(chartPlaceholder, 1);

    // 设置contentArea的布局
    contentArea->setLayout(pageLayout);
}