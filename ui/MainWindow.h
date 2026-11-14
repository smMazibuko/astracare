#pragma once
// Owner: Role 2 (UI Lead)

#include <QMainWindow>

#include "astracare/alert_engine.h"
#include "astracare/database.h"

class QListWidget;
class QStackedWidget;
class AlertsPage;
class DashboardPage;
class TrackPage;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void refreshFromCore();

    QListWidget* nav_;
    QStackedWidget* pages_;
    AlertsPage* alertsPage_;
    DashboardPage* dashboardPage_;
    TrackPage* trackPage_;
    astracare::Database database_;
    astracare::AlertEngine alertEngine_;
};
