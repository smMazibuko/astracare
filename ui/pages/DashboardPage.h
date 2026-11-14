#pragma once
// Owner: Role 2 (UI Lead)

#include <QElapsedTimer>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <vector>

#include "astracare/models.h"
#include "widgets/AlertBanner.h"
#include "widgets/BodyTwinWidget.h"
#include "widgets/VitalCard.h"

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);

public slots:
    void setReadings(const std::vector<astracare::VitalReading>& readings);
    void setActiveAlert(const astracare::Alert& topAlert);

signals:
    void navigateToPage(int pageIndex);
    void openGuidanceById(const QString& guidanceId);

private slots:
    void onZoneSelected(const QString& zone);
    void startPvtTest();
    void handlePvtTap();

private:
    AlertBanner* alertBanner_;
    BodyTwinWidget* bodyTwin_;

    VitalCard* hrCard_;
    VitalCard* bpCard_;
    VitalCard* sleepCard_;
    VitalCard* moodCard_;

    QLineSeries* hrSeries_;
    QLineSeries* sysSeries_;
    QValueAxis* axisX_;

    QLabel* zoneTitleLabel_;
    QLabel* zoneSummaryLabel_;
    QLabel* zoneMetric1Label_;
    QLabel* zoneMetric2Label_;

    QLabel* pvtStatusLabel_;
    QPushButton* pvtBtn_;
    QTimer* pvtWaitTimer_;
    QElapsedTimer pvtStopwatch_;
    int pvtPhase_ = 0;

    bool hasReading_ = false;
    astracare::VitalReading latestReading_;
};