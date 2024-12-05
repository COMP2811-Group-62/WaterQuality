#include "litterpage.h"
#include <QGridLayout>
#include <QLabel>
#include <set>
#include "styles.h"

LitterPage::LitterPage(QWidget *parent)
    : BasePage("Litter Indicators", parent)
{
    loadData();
    setupUI();
    setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                              ":/styles/litter.qss"}));
}

void LitterPage::loadData()
{
    dataset.loadData("../dataset/Y-2024-M.csv");
}

void LitterPage::setupUI()
{
    mainLayout = new QVBoxLayout(contentArea);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("Environmental Litter Analysis");
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    setupFilters();
    setupCharts();

    updateCharts();
}

void LitterPage::setupFilters()
{
    // 创建Frame容器及其布局
    controlsFrame = new QFrame();
    controlsFrame->setObjectName("controlsFrame");
    controlsLayout = new QHBoxLayout(controlsFrame);
    controlsLayout->setSpacing(20);

    // 创建标签和下拉框
    QLabel *locationLabel = new QLabel("Location:");
    locationLabel->setObjectName("filterLabel");
    locationFilter = new QComboBox();

    QLabel *typeLabel = new QLabel("Water Type:");
    typeLabel->setObjectName("filterLabel");
    waterTypeFilter = new QComboBox();

    // 填充下拉框数据
    std::set<QString> locations;
    std::set<QString> waterTypes;

    for (int i = 0; i < dataset.size(); i++)
    {
        locations.insert(QString::fromStdString(dataset[i].getSamplingPointLabel()));
        waterTypes.insert(QString::fromStdString(dataset[i].getSampledMaterialType()));
    }

    locationFilter->addItem("All Locations");
    for (const auto &loc : locations)
    {
        locationFilter->addItem(loc);
    }

    waterTypeFilter->addItem("All Types");
    for (const auto &type : waterTypes)
    {
        waterTypeFilter->addItem(type);
    }

    // 添加控件到controlsLayout
    controlsLayout->addWidget(locationLabel);
    controlsLayout->addWidget(locationFilter);
    controlsLayout->addSpacing(20);
    controlsLayout->addWidget(typeLabel);
    controlsLayout->addWidget(waterTypeFilter);
    controlsLayout->addStretch();

    // 将Frame添加到主布局
    mainLayout->addWidget(controlsFrame);

    // 连接信号
    connect(locationFilter, &QComboBox::currentTextChanged,
            this, &LitterPage::onLocationFilterChanged);
    connect(waterTypeFilter, &QComboBox::currentTextChanged,
            this, &LitterPage::onWaterTypeFilterChanged);
}

void LitterPage::setupCharts()
{
    chartsLayout = new QHBoxLayout();

    // 创建Frame容器
    QFrame* barChartFrame = new QFrame();
    QFrame* pieChartFrame = new QFrame();
    
    // 设置objectName以便应用qss
    barChartFrame->setObjectName("chartFrame");
    pieChartFrame->setObjectName("chartFrame");

    QVBoxLayout* barLayout = new QVBoxLayout(barChartFrame);
    QVBoxLayout* pieLayout = new QVBoxLayout(pieChartFrame);

    // 创建图表
    barChart = new QChart();
    barChart->setTitle("Litter by Location");
    locationBarChart = new QChartView(barChart);
    locationBarChart->setRenderHint(QPainter::Antialiasing);
    locationBarChart->setDragMode(QGraphicsView::NoDrag);

    pieChart = new QChart();
    pieChart->setTitle("Distribution by Water Type");
    waterTypePieChart = new QChartView(pieChart);
    waterTypePieChart->setRenderHint(QPainter::Antialiasing);
    waterTypePieChart->setDragMode(QGraphicsView::NoDrag);

    // 添加到各自的布局
    barLayout->addWidget(locationBarChart);
    pieLayout->addWidget(waterTypePieChart);

    // 添加Frame到主布局
    chartsLayout->addWidget(barChartFrame);
    chartsLayout->addWidget(pieChartFrame);

    mainLayout->addLayout(chartsLayout);
}

void LitterPage::updateCharts()
{
    barChart->removeAllSeries();
    pieChart->removeAllSeries();

    QBarSeries *barSeries = new QBarSeries();
    QPieSeries *pieSeries = new QPieSeries();

    std::map<QString, int> locationCounts;
    std::map<QString, int> waterTypeCounts;

    QString selectedLocation = locationFilter->currentText();
    QString selectedWaterType = waterTypeFilter->currentText();

    for (int i = 0; i < dataset.size(); i++)
    {
        QString location = QString::fromStdString(dataset[i].getSamplingPointLabel());
        QString waterType = QString::fromStdString(dataset[i].getSampledMaterialType());

        if ((selectedLocation == "All Locations" || selectedLocation == location) &&
            (selectedWaterType == "All Types" || selectedWaterType == waterType))
        {
            locationCounts[location]++;
            waterTypeCounts[waterType]++;
        }
    }

    QBarSet *barSet = new QBarSet("Sample Count");
    QStringList categories;
    for (const auto &pair : locationCounts)
    {
        *barSet << pair.second;
        categories << pair.first;
    }
    barSeries->append(barSet);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    barChart->addSeries(barSeries);
    barChart->setAxisX(axisX, barSeries);
    barChart->createDefaultAxes();

    for (const auto &pair : waterTypeCounts)
    {
        pieSeries->append(pair.first, pair.second);
    }
    pieChart->addSeries(pieSeries);
}

void LitterPage::onLocationFilterChanged(const QString &)
{
    updateCharts();
}

void LitterPage::onWaterTypeFilterChanged(const QString &)
{
    updateCharts();
}
