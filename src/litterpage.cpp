#include "litterpage.h"
#include <QGridLayout>
#include <QLabel>
#include <set>

LitterPage::LitterPage(QWidget* parent)
    : BasePage("Litter Indicators", parent) {
    loadData();
    setupUI();
}

void LitterPage::loadData() {
    dataset.loadData("../dataset/Y-2024-M.csv");
}

void LitterPage::setupUI() {
    mainLayout = new QVBoxLayout(contentArea);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel* titleLabel = new QLabel("Environmental Litter Analysis");
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    setupFilters();
    setupCharts();

    updateCharts();
}

void LitterPage::setupFilters() {
    filtersLayout = new QHBoxLayout();
    
    QLabel* locationLabel = new QLabel("Location:");
    locationFilter = new QComboBox();

    QLabel* typeLabel = new QLabel("Water Type:");
    waterTypeFilter = new QComboBox();
    
    std::set<QString> locations;
    std::set<QString> waterTypes;
    
    for(int i = 0; i < dataset.size(); i++) {
        locations.insert(QString::fromStdString(dataset[i].getSamplingPointLabel()));
        waterTypes.insert(QString::fromStdString(dataset[i].getSampledMaterialType()));
    }
    
    locationFilter->addItem("All Locations");
    for(const auto& loc : locations) {
        locationFilter->addItem(loc);
    }
    
    waterTypeFilter->addItem("All Types");
    for(const auto& type : waterTypes) {
        waterTypeFilter->addItem(type);
    }
    
    filtersLayout->addWidget(locationLabel);
    filtersLayout->addWidget(locationFilter);
    filtersLayout->addSpacing(20);
    filtersLayout->addWidget(typeLabel);
    filtersLayout->addWidget(waterTypeFilter);
    filtersLayout->addStretch();
    
    mainLayout->addLayout(filtersLayout);
    
    connect(locationFilter, &QComboBox::currentTextChanged,
            this, &LitterPage::onLocationFilterChanged);
    connect(waterTypeFilter, &QComboBox::currentTextChanged,
            this, &LitterPage::onWaterTypeFilterChanged);
}

void LitterPage::setupCharts() {
    chartsLayout = new QHBoxLayout();
    
    barChart = new QChart();
    barChart->setTitle("Litter by Location");
    locationBarChart = new QChartView(barChart);
    locationBarChart->setRenderHint(QPainter::Antialiasing);
    
    pieChart = new QChart();
    pieChart->setTitle("Distribution by Water Type");
    waterTypePieChart = new QChartView(pieChart);
    waterTypePieChart->setRenderHint(QPainter::Antialiasing);
    
    chartsLayout->addWidget(locationBarChart);
    chartsLayout->addWidget(waterTypePieChart);
    
    mainLayout->addLayout(chartsLayout);
}

void LitterPage::updateCharts() {
    barChart->removeAllSeries();
    pieChart->removeAllSeries();
    
    QBarSeries* barSeries = new QBarSeries();
    QPieSeries* pieSeries = new QPieSeries();
    
    std::map<QString, int> locationCounts;
    std::map<QString, int> waterTypeCounts;
    
    QString selectedLocation = locationFilter->currentText();
    QString selectedWaterType = waterTypeFilter->currentText();
    
    for(int i = 0; i < dataset.size(); i++) {
        QString location = QString::fromStdString(dataset[i].getSamplingPointLabel());
        QString waterType = QString::fromStdString(dataset[i].getSampledMaterialType());
        
        if((selectedLocation == "All Locations" || selectedLocation == location) &&
           (selectedWaterType == "All Types" || selectedWaterType == waterType)) {
            locationCounts[location]++;
            waterTypeCounts[waterType]++;
        }
    }
    
    QBarSet* barSet = new QBarSet("Sample Count");
    QStringList categories;
    for(const auto& pair : locationCounts) {
        *barSet << pair.second;
        categories << pair.first;
    }
    barSeries->append(barSet);
    
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    barChart->addSeries(barSeries);
    barChart->setAxisX(axisX, barSeries);
    barChart->createDefaultAxes();
    
    for(const auto& pair : waterTypeCounts) {
        pieSeries->append(pair.first, pair.second);
    }
    pieChart->addSeries(pieSeries);
}

void LitterPage::onLocationFilterChanged(const QString&) {
    updateCharts();
}

void LitterPage::onWaterTypeFilterChanged(const QString&) {
    updateCharts();
}
