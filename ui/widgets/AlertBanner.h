#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "astracare/models.h"

class AlertBanner : public QFrame {
    Q_OBJECT
public:
    explicit AlertBanner(QWidget* parent = nullptr);

    void setAlert(const astracare::Alert& alert);

signals:
    void viewGuidanceRequested(const QString& guidanceId);
    void emergencyRequested();

private:
    QLabel* badgeLabel_;
    QLabel* titleLabel_;
    QLabel* msgLabel_;
    QPushButton* guideBtn_;
    QPushButton* sosBtn_;
    QString currentGuidanceId_;
};