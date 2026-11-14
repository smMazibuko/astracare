#include "MainWindow.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>

#include "pages/AlertsPage.h"
#include "pages/DashboardPage.h"
#include "pages/EmergencyPage.h"
#include "pages/GuidePage.h"
#include "pages/ReportsPage.h"
#include "pages/TrackPage.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Astracare • Autonomous Crew Health");
    resize(1240, 750);

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

    auto* brandTitle = new QLabel("ASTRACARE  <span style='color:#00d2ff; font-size:12px;'>CDS-EDGE</span>");
    brandTitle->setStyleSheet("color: #ffffff; font-size: 17px; font-weight: 800; letter-spacing: 1px;");

    auto* offlineBadge = new QLabel("● 100% OFFLINE / AIR-GAPPED");
    offlineBadge->setObjectName("OfflineBadge");

    auto* quickSosBtn = new QPushButton("SOS / EMERGENCY");
    quickSosBtn->setObjectName("SosBtn");

    headerLayout->addWidget(brandTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(offlineBadge);
    headerLayout->addSpacing(10);
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
        pages_->addWidget(page);
    };

    auto* dashboardPage = new DashboardPage;
    addPage("Dashboard", dashboardPage);
    addPage("Track", new TrackPage);
    addPage("Guide", new GuidePage);
    addPage("Alerts", new AlertsPage);
    addPage("Reports", new ReportsPage);
    addPage("Emergency", new EmergencyPage);

    connect(nav_, &QListWidget::currentRowChanged, pages_, &QStackedWidget::setCurrentIndex);
    connect(quickSosBtn, &QPushButton::clicked, this, [this]() { nav_->setCurrentRow(5); });
    connect(dashboardPage, &DashboardPage::navigateToPage, this, [this](int index) {
        nav_->setCurrentRow(index);
    });
    connect(dashboardPage, &DashboardPage::openGuidanceById, this, [this](const QString&) {
        nav_->setCurrentRow(2); // Jumps directly to Guide page
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
    auto* disclaimer = new QLabel("Disclaimer: Astracare supports astronauts; it does not replace doctors. Provides health guidance and alerts, not medical diagnoses.");
    disclaimer->setStyleSheet("color: #6b7280; font-size: 11px;");
    disclaimer->setAlignment(Qt::AlignCenter);
    footerLayout->addWidget(disclaimer);

    rootLayout->addWidget(header);
    rootLayout->addLayout(bodyLayout, 1);
    rootLayout->addWidget(footer);

    setCentralWidget(central);
}
