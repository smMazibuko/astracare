#include "MainWindow.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QListWidget>
#include <QScrollArea>
#include <QStackedWidget>
#include <QWidget>
#include <QDateTime>

#include "astracare/report.h"
#include "pages/AlertsPage.h"
#include "pages/DashboardPage.h"
#include "pages/EmergencyPage.h"
#include "pages/GuidePage.h"
#include "pages/ReportsPage.h"
#include "pages/TrackPage.h"
#include "astracare/report.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Astracare • Autonomous Crew Health");
    resize(1180, 700);
    setMinimumSize(900, 600);

    database_.open("astracare.db");

    auto* central = new QWidget;
    central->setObjectName("CentralContainer");
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Top Flight Deck Header
    auto* header = new QFrame;
    header->setObjectName("FlightHeader");
    header->setFixedHeight(58);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    auto* brandTitle =
        new QLabel("ASTRACARE  <span style='color:#2f8f5b; font-size:12px;'>CDS-EDGE</span>");
    brandTitle->setObjectName("BrandTitle");

    auto* quickSosBtn = new QPushButton("SOS / EMERGENCY");
    quickSosBtn->setObjectName("SosBtn");

    headerLayout->addWidget(brandTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(quickSosBtn);

    // Middle Workspace (Sidebar + Stacked Pages)
    auto* bodyLayout = new QHBoxLayout;
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    nav_ = new QListWidget;
    nav_->setObjectName("SideNav");
    nav_->setFixedWidth(190);

    pages_ = new QStackedWidget;

    auto addPage = [this](const QString& name, QWidget* page) {
        nav_->addItem(name);
        auto* scroll = new QScrollArea;
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setWidget(page);
        pages_->addWidget(scroll);
    };

    dashboardPage_ = new DashboardPage;
    trackPage_ = new TrackPage;
    alertsPage_ = new AlertsPage;
    guidePage_ = new GuidePage;
    emergencyPage_ = new EmergencyPage;
    addPage("Dashboard", dashboardPage_);
    addPage("Track", trackPage_);
    addPage("Guide", guidePage_);
    addPage("Alerts", alertsPage_);
    reportsPage_ = new ReportsPage;
    addPage("Reports", reportsPage_);
    addPage("Emergency", emergencyPage_);

    QString guidancePath =
        QDir::cleanPath(QDir(QCoreApplication::applicationDirPath())
                            .filePath("../../../resources/guidance/guidance.json"));
    if (!QFile::exists(guidancePath))
        guidancePath = QDir::current().filePath("resources/guidance/guidance.json");

    if (guidanceLibrary_.loadFromFile(guidancePath.toStdString())) {
        guidePage_->setGuidanceEntries(guidanceLibrary_.all());

        std::vector<astracare::GuidanceEntry> emergencyProtocols;
        for (const auto& entry : guidanceLibrary_.all()) {
            if (entry.category == "emergency")
                emergencyProtocols.push_back(entry);
        }
        emergencyPage_->setEmergencyProtocols(emergencyProtocols);
    }

    connect(nav_, &QListWidget::currentRowChanged, pages_, &QStackedWidget::setCurrentIndex);
    connect(quickSosBtn, &QPushButton::clicked, this, [this]() { nav_->setCurrentRow(5); });
    connect(dashboardPage_, &DashboardPage::navigateToPage, this,
            [this](int index) { nav_->setCurrentRow(index); });
    connect(dashboardPage_, &DashboardPage::openGuidanceById, this,
            [this](const QString& guidanceId) { openGuidance(guidanceId); });
    connect(alertsPage_, &AlertsPage::openGuidanceRequested, this,
            [this](const QString& guidanceId) { openGuidance(guidanceId); });
    connect(alertsPage_, &AlertsPage::emergencyRequested, this,
            [this]() { nav_->setCurrentRow(5); });

    connect(trackPage_, &TrackPage::readingLogged, this,
            [this](const astracare::VitalReading& reading) {
                database_.insertReading(reading);
                refreshFromCore();
            });

    connect(reportsPage_, &ReportsPage::generateReportRequested, this, [this]() {
        const auto readings = database_.allReadings();
        astracare::MedicalReport report;
        report.generatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        report.readings = readings;
        if (!readings.empty()) {
            report.alerts = alertEngine_.evaluate(readings.back());
            const auto trends = alertEngine_.evaluateTrend(readings);
            report.alerts.insert(report.alerts.end(), trends.begin(), trends.end());
            for (const auto& alert : report.alerts) {
                if (!alert.guidanceId.empty()) report.guidanceIds.push_back(alert.guidanceId);
            }
        }
        report.notes = "Generated offline from the local SQLite health history.";
        reportsPage_->setReportData(QString::fromStdString(astracare::ReportBuilder::toJson(report)),
                                     QString::fromStdString(astracare::ReportBuilder::toCsv(report)));
    });

    nav_->setCurrentRow(0);

    bodyLayout->addWidget(nav_);
    bodyLayout->addWidget(pages_, 1);

    // Medical Disclaimer Footer
    auto* footer = new QFrame;
    footer->setObjectName("DisclaimerFooter");
    footer->setFixedHeight(32);
    auto* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(20, 0, 20, 0);
    auto* disclaimer =
        new QLabel("Disclaimer: Astracare supports astronauts; it does not replace doctors. "
                   "Provides health guidance and alerts, not medical diagnoses.");
    disclaimer->setObjectName("DisclaimerText");
    disclaimer->setAlignment(Qt::AlignCenter);
    footerLayout->addWidget(disclaimer);

    rootLayout->addWidget(header);
    rootLayout->addLayout(bodyLayout, 1);
    rootLayout->addWidget(footer);

    setCentralWidget(central);
    refreshFromCore();
}

void MainWindow::openGuidance(const QString& guidanceId) {
    QString resolvedId = guidanceId;
    if (!guidanceLibrary_.findById(guidanceId.toStdString())) {
        QString category;
        if (guidanceId.startsWith("cardiac"))
            category = "cardiac";
        else if (guidanceId.startsWith("sleep"))
            category = "sleep";
        else if (guidanceId.startsWith("vision"))
            category = "vision";
        else if (guidanceId.startsWith("neuro"))
            category = "sensorimotor";

        for (const auto& entry : guidanceLibrary_.all()) {
            if (QString::fromStdString(entry.category) == category) {
                resolvedId = QString::fromStdString(entry.id);
                break;
            }
        }
    }
    guidePage_->selectGuidanceById(resolvedId);
    nav_->setCurrentRow(2);
}

void MainWindow::refreshFromCore() {
    const auto readings = database_.allReadings();
    trackPage_->setReadings(readings);
    dashboardPage_->setReadings(readings);

    std::vector<astracare::Alert> alerts;
    if (!readings.empty()) {
        alerts = alertEngine_.evaluate(readings.back());
        const auto trends = alertEngine_.evaluateTrend(readings);
        alerts.insert(alerts.end(), trends.begin(), trends.end());
    }
    alertsPage_->setAlerts(alerts);
    if (!alerts.empty())
        dashboardPage_->setActiveAlert(alerts.front());
    else
        dashboardPage_->setActiveAlert({});
}
