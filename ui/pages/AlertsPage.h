#pragma once
// Owner: Role 4 (Alerts Engineer)

#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>
#include "astracare/models.h"

class AlertsPage : public QWidget {
    Q_OBJECT
public:
    explicit AlertsPage(QWidget* parent = nullptr);

public slots:
    void setAlerts(const std::vector<astracare::Alert>& alerts);
    void addTriggeredAlert(const astracare::Alert& alert);
    void setThresholds(const std::vector<std::array<QString, 3>>& rows);

signals:
    void openGuidanceRequested(const QString& guidanceId);
    void emergencyRequested();

private:
    QVBoxLayout* alertsListLayout_;
    QLabel* emptyStateLabel_;
    QTableWidget* rangeTable_;
};
