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
    QFrame *barChartFrame = new QFrame();
    QFrame *pieChartFrame = new QFrame();

    // 设置objectName以便应用qss
    barChartFrame->setObjectName("chartFrame");
    pieChartFrame->setObjectName("chartFrame");

    QVBoxLayout *barLayout = new QVBoxLayout(barChartFrame);
    QVBoxLayout *pieLayout = new QVBoxLayout(pieChartFrame);

    // 创建图表
    barChart = new QChart();
    barChart->setTitle("Litter by Location");
    locationBarChart = new QChartView(barChart);
    locationBarChart->setRenderHint(QPainter::Antialiasing);
    locationBarChart->setDragMode(QGraphicsView::NoDrag);

    locationBarChart->setInteractive(true);

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

    connect(barSeries, &QBarSeries::hovered, 
            this, &LitterPage::onBarHovered);
    

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

void LitterPage::onBarHovered(bool status, int index, QBarSet *barset)
{
    // qDebug() << "Hover event triggered:" << status << index; // 添加这行

    if (!status)
    {
        QToolTip::hideText();
        return;
    }

    // 从存储的属性中获取类别列表
    QStringList categories = barChart->property("categories").toStringList();
    if (index >= 0 && index < categories.size())
    {
        QString location = categories.at(index);
        double value = barset->at(index);

        // 获取该位置的坐标
        QString coordinates = getLocationCoordinates(location);

        // 创建工具提示文本
        QString tooltip = QString(
                              "Location: %1\n"
                              "Samples: %2\n"
                              "Coordinates: %3")
                              .arg(location)
                              .arg(value)
                              .arg(coordinates);

        QToolTip::showText(QCursor::pos(), tooltip);
    }
}

QString LitterPage::getLocationCoordinates(const QString &location) const
{
    for (int i = 0; i < dataset.size(); i++)
    {
        if (QString::fromStdString(dataset[i].getSamplingPointLabel()) == location)
        {
            QString easting = QString::fromStdString(dataset[i].getEasting());
            QString northing = QString::fromStdString(dataset[i].getNorthing());
            return QString("E: %1, N: %2").arg(easting).arg(northing);
        }
    }
    return "N/A";
}