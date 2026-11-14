#pragma once
// Owner: Role 3 (Vitals Tracking)

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QList>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>
#include <vector>
#include "astracare/models.h"

class TrackPage : public QWidget {
    Q_OBJECT
public:
    explicit TrackPage(QWidget* parent = nullptr);
    
public slots:
    void setReadings(const std::vector<astracare::VitalReading>& readings);

signals:
    void readingLogged(const astracare::VitalReading& reading);

private slots:
    void onSaveClicked();

private:
    void appendRowToTable(const astracare::VitalReading& r);

    QSpinBox* hrInput_;
    QSpinBox* sysInput_;
    QSpinBox* diaInput_;
    QDoubleSpinBox* sleepInput_;
    QSpinBox* moodInput_;
    QList<QCheckBox*> symptomChecks_;
    QTableWidget* historyTable_;
};
