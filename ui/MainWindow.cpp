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
    setWindowTitle("Astracare");
    resize(1000, 650);

    nav_ = new QListWidget;
    nav_->setFixedWidth(180);
    pages_ = new QStackedWidget;

    auto addPage = [this](const QString& name, QWidget* page) {
        nav_->addItem(name);
        pages_->addWidget(page);
    };

    addPage("Dashboard", new DashboardPage);
    addPage("Track", new TrackPage);
    addPage("Guide", new GuidePage);
    addPage("Alerts", new AlertsPage);
    addPage("Reports", new ReportsPage);
    addPage("Emergency", new EmergencyPage);

    connect(nav_, &QListWidget::currentRowChanged, pages_, &QStackedWidget::setCurrentIndex);
    nav_->setCurrentRow(0);

    auto* central = new QWidget;
    auto* layout = new QHBoxLayout(central);
    layout->addWidget(nav_);
    layout->addWidget(pages_, 1);
    setCentralWidget(central);
}
