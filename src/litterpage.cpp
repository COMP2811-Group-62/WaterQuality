#include "litterpage.h"
#include <QGridLayout>
#include <QLabel>
#include <set>
#include "styles.h"

LitterPage::LitterPage(QWidget *parent)
    : BasePage("Litter Indicators", parent)
{
    model.updateFromFile("../dataset/Y-2024-M.csv");
    setupUI();
    updateLocationCompleter();
    setStyleSheet(Styles::combineStyleSheets({":/styles/basepage.qss",
                                              ":/styles/litter.qss"}));

    updateCharts(); // 添加这一行
}

void LitterPage::setupUI()
{
    // 左侧面板
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setSpacing(20);

    // 标题
    QLabel *titleLabel = new QLabel("Environmental Litter Analysis", leftPanel);
    titleLabel->setObjectName("pageTitle");
    leftPanelLayout->addWidget(titleLabel);

    // 设置过滤器和图表
    setupFilters();
    leftPanelLayout->addWidget(controlsFrame); // 添加过滤器框架

    setupCharts();
    leftPanelLayout->addLayout(chartsLayout); // 添加图表布局

    // 设置信息面板
    setupInfoPanel();

    // 创建主水平布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(leftPanel, 3); // 左侧占据更多空间
    horizontalLayout->addWidget(infoPanel, 1); // 右侧信息面板占据较少空间

    // 设置主布局
    mainLayout = new QVBoxLayout(contentArea); // 注意这里设置了 parent
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(horizontalLayout);
}

void LitterPage::setupFilters()
{
    controlsFrame = new QFrame();
    controlsFrame->setObjectName("controlsFrame");
    controlsLayout = new QHBoxLayout(controlsFrame);
    controlsLayout->setSpacing(20);

    // Pollutant type filter
    QLabel *typeLabel = new QLabel("Pollutant Type:");
    typeLabel->setObjectName("filterLabel");
    waterTypeFilter = new QComboBox();
    waterTypeFilter->addItem("All Types");
    waterTypeFilter->addItem("Plastic Waste");
    waterTypeFilter->addItem("Sewage Debris");
    waterTypeFilter->setMaximumWidth(200);

    // Location search filter
    QLabel *locationLabel = new QLabel("Location Search:");
    locationLabel->setObjectName("filterLabel");
    locationSearch = new QLineEdit();
    locationSearch->setPlaceholderText("Enter location name to search...");
    locationSearch->setMaximumWidth(200);

    // Create location auto-completer
    locationCompleter = new QCompleter(this);
    locationCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    locationCompleter->setFilterMode(Qt::MatchContains);
    locationSearch->setCompleter(locationCompleter);

    controlsLayout->addWidget(typeLabel);
    controlsLayout->addWidget(waterTypeFilter);
    controlsLayout->addWidget(locationLabel);
    controlsLayout->addWidget(locationSearch);
    controlsLayout->addStretch();

    // mainLayout->addWidget(controlsFrame);

    // Connect signals
    connect(waterTypeFilter, &QComboBox::currentTextChanged,
            this, &LitterPage::updateCharts);
    connect(locationSearch, &QLineEdit::textChanged,
            this, &LitterPage::onLocationSearchChanged);
    connect(locationCompleter,
            QOverload<const QString &>::of(&QCompleter::activated),
            this, &LitterPage::onLocationSelected);
}

void LitterPage::setupCharts()
{
    chartsLayout = new QHBoxLayout();

    // Create Frame container
    QFrame *barChartFrame = new QFrame();
    barChartFrame->setObjectName("chartFrame");
    barChartFrame->setMinimumSize(1200, 400);

    QVBoxLayout *barLayout = new QVBoxLayout(barChartFrame);

    // Create bar chart
    barChart = new QChart();
    barChart->setTitle("Pollutant Level Distribution");
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    barChart->legend()->setVisible(true);
    barChart->legend()->setAlignment(Qt::AlignBottom);
    barChart->setMargins(QMargins(10, 10, 10, 10));
    barChart->layout()->setContentsMargins(0, 0, 0, 0);

    locationBarChart = new QChartView(barChart);
    locationBarChart->setRenderHint(QPainter::Antialiasing);
    locationBarChart->setInteractive(true);

    barLayout->addWidget(locationBarChart);
    chartsLayout->addWidget(barChartFrame);
    // mainLayout->addLayout(chartsLayout);
}

void LitterPage::updateCharts()
{
    locationLevels.clear();

    // Get filter conditions
    QString selectedType = waterTypeFilter->currentText();
    QString searchLocation = locationSearch->text().trimmed();

    // Clear existing charts
    barChart->removeAllSeries();

    // Remove all axes
    QList<QAbstractAxis *> axes = barChart->axes();
    for (QAbstractAxis *axis : axes)
    {
        barChart->removeAxis(axis);
    }

    // Create two bar sets
    QBarSet *plasticSet = new QBarSet("Plastic Waste");
    QBarSet *sewageSet = new QBarSet("Sewage Debris");
    sewageSet->setColor(QColor("#4CAF50"));

    QBarSeries *barSeries = new QBarSeries();
    barSeries->setBarWidth(0.85);
    barSeries->setLabelsVisible(true);
    barSeries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);

    // Iterate through data set and apply filter conditions
    for (int i = 0; i < model.rowCount(QModelIndex()); i++)
    {
        QString location = model.data(model.index(i, 2), Qt::DisplayRole).toString();

        // Apply location search filter
        if (!searchLocation.isEmpty() &&
            !location.contains(searchLocation, Qt::CaseInsensitive))
        {
            continue;
        }

        QString litterType = model.data(model.index(i, 4), Qt::DisplayRole).toString();
        QString litterDef = model.data(model.index(i, 5), Qt::DisplayRole).toString();
        QString resultStr = model.data(model.index(i, 7), Qt::DisplayRole).toString();

        bool isPlastic = litterDef.contains("Plastic", Qt::CaseInsensitive) ||
                         litterType.contains("Plastic", Qt::CaseInsensitive);
        bool isSewage = litterDef.contains("Sewage", Qt::CaseInsensitive) ||
                        litterType.contains("Sewage", Qt::CaseInsensitive);

        if (!isPlastic && !isSewage)
        {
            continue;
        }

        QString standardType = isPlastic ? "Plastic Waste" : "Sewage Debris";

        if (selectedType != "All Types" && standardType != selectedType)
        {
            continue;
        }

        // Process value
        double value = 0.0;
        bool ok;
        value = resultStr.toDouble(&ok);
        if (!ok && resultStr.startsWith("<"))
        {
            value = resultStr.mid(1).toDouble(&ok) / 2.0;
        }
        if (!ok)
        {
            continue;
        }

        if (standardType == "Plastic Waste")
        {
            locationLevels[location].first += value;
        }
        else
        {
            locationLevels[location].second += value;
        }
    }

    // Prepare chart data
    QStringList categories;
    for (const auto &pair : locationLevels)
    {
        categories << pair.first;
        *plasticSet << pair.second.first;
        *sewageSet << pair.second.second;
    }

    if (selectedType == "All Types" || selectedType == "Plastic Waste")
    {
        barSeries->append(plasticSet);
    }
    if (selectedType == "All Types" || selectedType == "Sewage Debris")
    {
        barSeries->append(sewageSet);
    }

    // Set axes
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-30);
    axisX->setTitleText("Sampling Location");
    axisX->setLabelsFont(QFont("Arial", 9));

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Pollutant Level (mg/l)");
    axisY->setLabelFormat("%.3f");

    barChart->addSeries(barSeries);
    barChart->addAxis(axisX, Qt::AlignBottom);
    barChart->addAxis(axisY, Qt::AlignLeft);

    barSeries->attachAxis(axisX);
    barSeries->attachAxis(axisY);

    // 在这里添加阈值线代码 >>>
    // 创建警告阈值线
    warningThresholdLine = new QLineSeries();
    warningThresholdLine->setName("Warning Threshold");
    QPen warningPen(QColor("#ffd700")); // 黄色
    warningPen.setWidth(2);
    warningPen.setStyle(Qt::DashLine); // 虚线
    warningThresholdLine->setPen(warningPen);

    // 创建危险阈值线
    dangerThresholdLine = new QLineSeries();
    dangerThresholdLine->setName("Danger Threshold");
    QPen dangerPen(QColor("#ff4444")); // 红色
    dangerPen.setWidth(2);
    dangerPen.setStyle(Qt::DashLine);
    dangerThresholdLine->setPen(dangerPen);

    // 添加线的数据点
    if (!categories.isEmpty())
    {
        // 获取 x 轴的范围
        double xMin = -0.5;                    // 稍微向左偏移以覆盖整个图表
        double xMax = categories.size() - 0.5; // 稍微向右偏移

        // 添加警告阈值线的点
        warningThresholdLine->append(xMin, WARNING_THRESHOLD);
        warningThresholdLine->append(xMax, WARNING_THRESHOLD);

        // 添加危险阈值线的点
        dangerThresholdLine->append(xMin, DANGER_THRESHOLD);
        dangerThresholdLine->append(xMax, DANGER_THRESHOLD);

        // 添加到图表
        barChart->addSeries(warningThresholdLine);
        barChart->addSeries(dangerThresholdLine);

        // 将线附加到坐标轴
        warningThresholdLine->attachAxis(axisX);
        warningThresholdLine->attachAxis(axisY);
        dangerThresholdLine->attachAxis(axisX);
        dangerThresholdLine->attachAxis(axisY);
    }

    // 断开之前的连接
    disconnect(barSeries, &QBarSeries::hovered,
               this, &LitterPage::onBarHovered);

    // 重新连接信号
    connect(barSeries, &QBarSeries::hovered,
            this, &LitterPage::onBarHovered);

    qDebug() << "图表更新完成,数据点数量:" << barSeries->count();
}

void LitterPage::onWaterTypeFilterChanged(const QString &)
{
    updateCharts();
}

void LitterPage::onBarHovered(bool status, int index, QBarSet *barset)
{
    if (!status || index < 0 || !barset)
    {
        QToolTip::hideText();
        return;
    }

    QStringList categories;
    for (const auto &pair : locationLevels)
    {
        categories << pair.first;
    }

    if (index >= categories.size())
    {
        return;
    }

    QString location = categories.at(index);
    double value = barset->at(index);

    // 简化内容，使用纯文本而不是HTML
    QString tooltipText = QString("Location: %1\nValue: %2 mg/l\nPosition: (%3, %4)")
                              .arg(location)
                              .arg(value, 0, 'f', 3)
                              .arg(QCursor::pos().x())
                              .arg(QCursor::pos().y());

    // 获取图表视图中的位置
    QPoint pos = QCursor::pos();

    // 使用替代方法显示工具提示
    QToolTip::showText(pos, tooltipText, locationBarChart);

    // 添加调试输出
    qDebug() << "显示工具提示:" << tooltipText;
    qDebug() << "在位置:" << pos;
}

QString LitterPage::getLocationCoordinates(const QString &location) const
{
    for (int i = 0; i < model.rowCount(QModelIndex()); i++)
    {
        QString currentLocation = model.data(model.index(i, 2), Qt::DisplayRole).toString();
        if (currentLocation == location)
        {
            QString easting = model.data(model.index(i, 5), Qt::DisplayRole).toString();
            QString northing = model.data(model.index(i, 6), Qt::DisplayRole).toString();
            return QString("E: %1, N: %2").arg(easting).arg(northing);
        }
    }
    return "N/A";
}

void LitterPage::updateLocationCompleter()
{
    QSet<QString> locations;
    for (int i = 0; i < model.rowCount(QModelIndex()); i++)
    {
        QString location = model.data(model.index(i, 2), Qt::DisplayRole).toString();
        if (!location.isEmpty())
        {
            locations.insert(location);
        }
    }

    allLocations = locations.values();
    locationCompleter->setModel(new QStringListModel(allLocations));
}

void LitterPage::onLocationSearchChanged(const QString &)
{
    updateCharts();
}

void LitterPage::onLocationSelected(const QString &location)
{
    locationSearch->setText(location);
    updateCharts();
}

void LitterPage::setupInfoPanel()
{
    // 创建面板
    infoPanel = new QFrame(contentArea);
    infoPanel->setObjectName("infoPanel");
    infoPanel->setFixedWidth(300);

    QVBoxLayout *infoPanelLayout = new QVBoxLayout(infoPanel);
    infoPanelLayout->setSpacing(15);
    infoPanelLayout->setContentsMargins(15, 15, 15, 15);

    // 阈值信息框
    QFrame *thresholdFrame = new QFrame(infoPanel);
    thresholdFrame->setObjectName("thresholdFrame");

    QVBoxLayout *thresholdLayout = new QVBoxLayout(thresholdFrame);

    // 标题
    QLabel *thresholdTitle = new QLabel("Safety Thresholds", thresholdFrame);
    thresholdTitle->setStyleSheet("font-size: 16px; font-weight: bold;");

    // 内容
    QLabel *thresholdLabel = new QLabel(thresholdFrame);
    thresholdLabel->setWordWrap(true);
    thresholdLabel->setText(
        "UK/EU Safety Thresholds for Plastic Waste:\n"
        "• Safe: < 15.0 mg/L\n"
        "• Warning: 15.0-25.0 mg/L\n"
        "• Danger: > 25.0 mg/L");

    // 添加到布局
    thresholdLayout->addWidget(thresholdTitle);
    thresholdLayout->addWidget(thresholdLabel);

    infoPanelLayout->addWidget(thresholdFrame);
    infoPanelLayout->addStretch();
}