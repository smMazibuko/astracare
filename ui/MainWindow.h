#pragma once
// Owner: Role 2 (UI Lead)

#include <QMainWindow>

#include "astracare/alert_engine.h"
#include "astracare/database.h"
#include "astracare/guidance.h"

class QListWidget;
class QStackedWidget;
class AlertsPage;
class DashboardPage;
class EmergencyPage;
class GuidePage;
class TrackPage;
class ReportsPage;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void openGuidance(const QString& guidanceId);
    void refreshFromCore();

    QListWidget* nav_;
    QStackedWidget* pages_;
    AlertsPage* alertsPage_;
    DashboardPage* dashboardPage_;
    EmergencyPage* emergencyPage_;
    GuidePage* guidePage_;
    TrackPage* trackPage_;
    ReportsPage* reportsPage_;
    astracare::Database database_;
    astracare::AlertEngine alertEngine_;
    astracare::GuidanceLibrary guidanceLibrary_;
    astracare::GuidanceLibrary emergencyLibrary_;
};
