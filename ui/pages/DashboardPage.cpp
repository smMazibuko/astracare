#include "pages/DashboardPage.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <algorithm>

DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
   root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // 1. Dynamic Alert Banner (hidden when AlertLevel::Normal)
    alertBanner_ = new AlertBanner;
    root->addWidget(alertBanner_);

    // 2. Main 3-Column Flight Deck Grid
    auto* grid = new QHBoxLayout;
    grid->setSpacing(14);

    // LEFT COLUMN: Crew Digital Twin
    auto* twinCard = new QFrame;
    twinCard->setObjectName("SpaceCard");
    auto* twinLayout = new QVBoxLayout(twinCard);
    twinLayout->setContentsMargins(16, 14, 16, 14);

    auto* twinHeader = new QLabel("CREW DIGITAL TWIN");
    twinHeader->setStyleSheet("color: #d1d5db; font-weight: 700; font-size: 12px; letter-spacing: 0.6px;");
    auto* twinHint = new QLabel("Click body zone to inspect");
    twinHint->setStyleSheet("color: #6b7280; font-size: 11px;");

    bodyTwin_ = new BodyTwinWidget;

    auto* zoneBtnRow = new QHBoxLayout;
    auto* btnHead = new QPushButton("Head");
    auto* btnChest = new QPushButton("Chest");
    auto* btnLegs = new QPushButton("Legs");
    zoneBtnRow->addWidget(btnHead);
    zoneBtnRow->addWidget(btnChest);
    zoneBtnRow->addWidget(btnLegs);

    twinLayout->addWidget(twinHeader);
    twinLayout->addWidget(twinHint);
    twinLayout->addWidget(bodyTwin_, 1, Qt::AlignCenter);
    twinLayout->addLayout(zoneBtnRow);

    // CENTER COLUMN: Biometric Cards + QtCharts Trend + PVT Reaction Test
    auto* centerCol = new QVBoxLayout;
    centerCol->setSpacing(14);

    auto* vitalsCard = new QFrame;
    vitalsCard->setObjectName("SpaceCard");
    auto* vitalsLayout = new QVBoxLayout(vitalsCard);
    vitalsLayout->setContentsMargins(16, 14, 16, 14);

    auto* vitalsHeaderRow = new QHBoxLayout;
    auto* vitalsTitle = new QLabel("LATEST BIOMETRIC TELEMETRY");
    vitalsTitle->setStyleSheet("color: #d1d5db; font-weight: 700; font-size: 12px; letter-spacing: 0.6px;");
    auto* logBtn = new QPushButton("+ Log Vitals");
    logBtn->setObjectName("PrimaryBtn");
    vitalsHeaderRow->addWidget(vitalsTitle);
    vitalsHeaderRow->addStretch();
    vitalsHeaderRow->addWidget(logBtn);

    auto* cardsGrid = new QGridLayout;
    cardsGrid->setSpacing(10);
    hrCard_ = new VitalCard("Heart Rate", "BPM");
    bpCard_ = new VitalCard("Blood Pressure", "mmHg");
    sleepCard_ = new VitalCard("Sleep Recovery", "HRS");
    moodCard_ = new VitalCard("Wellbeing / Mood", "/ 5");
    cardsGrid->addWidget(hrCard_, 0, 0);
    cardsGrid->addWidget(bpCard_, 0, 1);
    cardsGrid->addWidget(sleepCard_, 1, 0);
    cardsGrid->addWidget(moodCard_, 1, 1);

    // QtCharts Trend Graph (starts empty until setReadings is called)
    hrSeries_ = new QLineSeries;
    hrSeries_->setName("Heart Rate (BPM)");
    hrSeries_->setColor(QColor("#10b981"));

    sysSeries_ = new QLineSeries;
    sysSeries_->setName("Systolic BP");
    sysSeries_->setColor(QColor("#00d2ff"));

    auto* chart = new QChart;
    chart->addSeries(hrSeries_);
    chart->addSeries(sysSeries_);
    chart->setBackgroundBrush(QBrush(QColor("#070a13")));
    chart->setTitle("Biometric Trend History");
    chart->setTitleBrush(QBrush(QColor("#9ca3af")));
    chart->legend()->setLabelColor(QColor("#d1d5db"));
    chart->setMargins(QMargins(4, 4, 4, 4));

    axisX_ = new QValueAxis;
    axisX_->setRange(1, 5);
    axisX_->setTickType(QValueAxis::TicksDynamic);
    axisX_->setTickInterval(1.0);
    axisX_->setLabelFormat("%d");
    axisX_->setLabelsColor(QColor("#6b7280"));
    axisX_->setGridLineColor(QColor("#1b2438"));

    auto* axisY = new QValueAxis;
    axisY->setRange(40, 180);
    axisY->setLabelsColor(QColor("#6b7280"));
    axisY->setGridLineColor(QColor("#1b2438"));

    chart->addAxis(axisX_, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    hrSeries_->attachAxis(axisX_);
    hrSeries_->attachAxis(axisY);
    sysSeries_->attachAxis(axisX_);
    sysSeries_->attachAxis(axisY);

    auto* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(195);

    vitalsLayout->addLayout(vitalsHeaderRow);
    vitalsLayout->addLayout(cardsGrid);
    vitalsLayout->addWidget(chartView, 1);

    // Interactive Cognitive Vigilance Test (PVT)
    auto* pvtCard = new QFrame;
    pvtCard->setObjectName("SpaceCard");
    auto* pvtLayout = new QHBoxLayout(pvtCard);
    pvtLayout->setContentsMargins(16, 12, 16, 12);

    auto* pvtTextCol = new QVBoxLayout;
    auto* pvtTitle = new QLabel("COGNITIVE VIGILANCE TEST (PVT)");
    pvtTitle->setStyleSheet("color: #ffffff; font-weight: 700; font-size: 12px;");
    pvtStatusLabel_ = new QLabel("Measures neuro-fatigue alertness via visual reaction speed.");
    pvtStatusLabel_->setStyleSheet("color: #9ca3af; font-size: 11px;");
    pvtTextCol->addWidget(pvtTitle);
    pvtTextCol->addWidget(pvtStatusLabel_);

    pvtBtn_ = new QPushButton("Start Reaction Check");
    pvtLayout->addLayout(pvtTextCol, 1);
    pvtLayout->addWidget(pvtBtn_);

    centerCol->addWidget(vitalsCard, 1);
    centerCol->addWidget(pvtCard);

    // RIGHT COLUMN: Selected Organ Diagnostic & Actions
    auto* diagCard = new QFrame;
    diagCard->setObjectName("SpaceCard");
    auto* diagLayout = new QVBoxLayout(diagCard);
    diagLayout->setContentsMargins(16, 14, 16, 14);
    diagLayout->setSpacing(10);

    auto* diagTag = new QLabel("ZONE DIAGNOSTIC");
    diagTag->setStyleSheet("color: #00d2ff; font-family: 'Consolas', monospace; font-size: 11px; font-weight: 700;");

    zoneTitleLabel_ = new QLabel("Cardiopulmonary & Hemodynamics");
    zoneTitleLabel_->setWordWrap(true);
    zoneTitleLabel_->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: 700;");

    zoneSummaryLabel_ = new QLabel("Awaiting telemetry readings from Track or storage.");
    zoneSummaryLabel_->setWordWrap(true);
    zoneSummaryLabel_->setStyleSheet("background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; padding: 12px; color: #d1d5db; font-size: 12px;");

    zoneMetric1Label_ = new QLabel("Metric 1: --");
    zoneMetric1Label_->setStyleSheet("background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; padding: 10px; color: #f3f4f6; font-family: 'Consolas', monospace; font-size: 12px;");

    zoneMetric2Label_ = new QLabel("Metric 2: --");
    zoneMetric2Label_->setStyleSheet("background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; padding: 10px; color: #f3f4f6; font-family: 'Consolas', monospace; font-size: 12px;");

    auto* guideBtn = new QPushButton("Open Offline Medical Guide");
    guideBtn->setObjectName("PrimaryBtn");

    auto* reportBtn = new QPushButton("Generate Flight Surgeon Report");

    diagLayout->addWidget(diagTag);
    diagLayout->addWidget(zoneTitleLabel_);
    diagLayout->addWidget(zoneSummaryLabel_);
    diagLayout->addWidget(zoneMetric1Label_);
    diagLayout->addWidget(zoneMetric2Label_);
    diagLayout->addStretch();
    diagLayout->addWidget(guideBtn);
    diagLayout->addWidget(reportBtn);

    grid->addWidget(twinCard, 3);
    grid->addLayout(centerCol, 5);
    grid->addWidget(diagCard, 3);
    root->addLayout(grid, 1);

    pvtWaitTimer_ = new QTimer(this);
    pvtWaitTimer_->setSingleShot(true);

    connect(bodyTwin_, &BodyTwinWidget::zoneSelected, this, &DashboardPage::onZoneSelected);
    connect(btnHead, &QPushButton::clicked, this, [this]() { bodyTwin_->setSelectedZone("head"); onZoneSelected("head"); });
    connect(btnChest, &QPushButton::clicked, this, [this]() { bodyTwin_->setSelectedZone("chest"); onZoneSelected("chest"); });
    connect(btnLegs, &QPushButton::clicked, this, [this]() { bodyTwin_->setSelectedZone("legs"); onZoneSelected("legs"); });

    connect(logBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(1); });
    connect(guideBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(2); });
    connect(reportBtn, &QPushButton::clicked, this, [this]() { emit navigateToPage(4); });

    connect(alertBanner_, &AlertBanner::viewGuidanceRequested, this, &DashboardPage::openGuidanceById);
    connect(alertBanner_, &AlertBanner::emergencyRequested, this, [this]() { emit navigateToPage(5); });

    connect(pvtBtn_, &QPushButton::clicked, this, [this]() {
        if (pvtPhase_ == 0) startPvtTest();
        else if (pvtPhase_ == 2) handlePvtTap();
    });
    connect(pvtWaitTimer_, &QTimer::timeout, this, [this]() {
        pvtPhase_ = 2;
        pvtBtn_->setText("TAP NOW!");
        pvtBtn_->setStyleSheet("background-color: #10b981; color: #070a13; font-weight: 800;");
        pvtStopwatch_.start();
    });

    // Initialize empty state
    hrCard_->setReading("--", "#6b7280", "No data");
    bpCard_->setReading("--/--", "#6b7280", "No data");
    sleepCard_->setReading("--", "#6b7280", "No data");
    moodCard_->setReading("--", "#6b7280", "No data");
}

void DashboardPage::setReadings(const std::vector<astracare::VitalReading>& readings) {
    hrSeries_->clear();
    sysSeries_->clear();

    if (readings.empty()) {
        hasReading_ = false;
        hrCard_->setReading("--", "#6b7280", "No data");
        bpCard_->setReading("--/--", "#6b7280", "No data");
        sleepCard_->setReading("--", "#6b7280", "No data");
        moodCard_->setReading("--", "#6b7280", "No data");
        bodyTwin_->setZoneStatuses(astracare::AlertLevel::Normal, astracare::AlertLevel::Normal, astracare::AlertLevel::Normal);
        return;
    }

    hasReading_ = true;
    latestReading_ = readings.back();

    // Populate chart from real reading history (last 10 entries)
    int startIdx = std::max(0, static_cast<int>(readings.size()) - 10);
    int pointX = 1;
    for (int i = startIdx; i < static_cast<int>(readings.size()); ++i) {
        hrSeries_->append(pointX, readings[i].heartRate);
        sysSeries_->append(pointX, readings[i].systolic);
        ++pointX;
    }
    axisX_->setRange(1, std::max(2, pointX - 1));

    // Update VitalCards
    bool highHr = latestReading_.heartRate > 100 || (latestReading_.heartRate > 0 && latestReading_.heartRate < 45);
    hrCard_->setReading(QString::number(latestReading_.heartRate), highHr ? "#ef4444" : "#10b981", highHr ? "Out of Range" : "Recorded");

    bool highBp = latestReading_.systolic >= 135 || latestReading_.diastolic >= 85;
    bpCard_->setReading(QString("%1/%2").arg(latestReading_.systolic).arg(latestReading_.diastolic), highBp ? "#f59e0b" : "#00d2ff", highBp ? "Elevated" : "Recorded");

    bool lowSleep = latestReading_.sleepHours < 6.0;
    sleepCard_->setReading(QString::number(latestReading_.sleepHours, 'f', 1), lowSleep ? "#f59e0b" : "#f3f4f6", lowSleep ? "Low Sleep" : "Recorded");

    moodCard_->setReading(QString::number(latestReading_.mood), latestReading_.mood <= 2 ? "#f59e0b" : "#10b981", "Recorded");

    // Update Body Twin zone colors from real reading
    auto headLvl = (latestReading_.sleepHours < 6.0 || !latestReading_.symptoms.empty())
                       ? astracare::AlertLevel::Caution
                       : astracare::AlertLevel::Normal;
    auto chestLvl = (latestReading_.heartRate > 110) ? astracare::AlertLevel::Urgent
                    : (latestReading_.heartRate > 95) ? astracare::AlertLevel::Caution
                                                      : astracare::AlertLevel::Normal;
    auto legsLvl = (latestReading_.systolic >= 138) ? astracare::AlertLevel::Caution : astracare::AlertLevel::Normal;

    bodyTwin_->setZoneStatuses(headLvl, chestLvl, legsLvl);
    onZoneSelected("chest");
}

void DashboardPage::setActiveAlert(const astracare::Alert& topAlert) {
    alertBanner_->setAlert(topAlert);
}

void DashboardPage::onZoneSelected(const QString& zone) {
    if (!hasReading_) {
        zoneSummaryLabel_->setText("No telemetry readings logged yet. Use Track to record vitals.");
        zoneMetric1Label_->setText("Metric 1: --");
        zoneMetric2Label_->setText("Metric 2: --");
        return;
    }

    if (zone == "head") {
        zoneTitleLabel_->setText("Neuro-Ocular & Vestibular (SANS)");
        zoneSummaryLabel_->setText("Displays latest sleep recovery and reported neurological/vestibular symptoms.");
        zoneMetric1Label_->setText(QString("Sleep Recovery: %1 hrs").arg(latestReading_.sleepHours, 0, 'f', 1));
        zoneMetric2Label_->setText(QString("Symptoms: %1").arg(latestReading_.symptoms.empty() ? "None" : QString::fromStdString(latestReading_.symptoms)));
    } else if (zone == "legs") {
        zoneTitleLabel_->setText("Fluid Shift & Musculoskeletal");
        zoneSummaryLabel_->setText("Displays hemodynamic pressure indicators associated with microgravity fluid shifts.");
        zoneMetric1Label_->setText(QString("Systolic Pressure: %1 mmHg").arg(latestReading_.systolic));
        zoneMetric2Label_->setText(QString("Diastolic Pressure: %1 mmHg").arg(latestReading_.diastolic));
    } else {
        zoneTitleLabel_->setText("Cardiopulmonary & Hemodynamics");
        zoneSummaryLabel_->setText("Displays latest logged heart rate and arterial blood pressure.");
        zoneMetric1Label_->setText(QString("Heart Rate: %1 BPM").arg(latestReading_.heartRate));
        zoneMetric2Label_->setText(QString("Blood Pressure: %1/%2 mmHg").arg(latestReading_.systolic).arg(latestReading_.diastolic));
    }
}

void DashboardPage::startPvtTest() {
    pvtPhase_ = 1;
    pvtBtn_->setText("WAIT FOR GREEN...");
    pvtBtn_->setStyleSheet("background-color: #f59e0b; color: #070a13; font-weight: 700;");
    int delayMs = QRandomGenerator::global()->bounded(1200, 2800);
    pvtWaitTimer_->start(delayMs);
}

void DashboardPage::handlePvtTap() {
    qint64 ms = pvtStopwatch_.elapsed();
    pvtPhase_ = 0;
    pvtBtn_->setText("Retest Reaction");
    pvtBtn_->setStyleSheet("");
    pvtStatusLabel_->setText(QString("Reaction Speed: %1 ms").arg(ms));
}