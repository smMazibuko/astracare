#pragma once

#include <QWidget>
#include <QRect>
#include <QString>
#include "astracare/models.h"

class BodyTwinWidget : public QWidget {
    Q_OBJECT
public:
    explicit BodyTwinWidget(QWidget* parent = nullptr);

    void setZoneStatuses(astracare::AlertLevel head, astracare::AlertLevel chest, astracare::AlertLevel legs);
    void setSelectedZone(const QString& zone);

signals:
    void zoneSelected(const QString& zone);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QColor colorForLevel(astracare::AlertLevel level) const;

    astracare::AlertLevel headLevel_ = astracare::AlertLevel::Normal;
    astracare::AlertLevel chestLevel_ = astracare::AlertLevel::Normal;
    astracare::AlertLevel legsLevel_ = astracare::AlertLevel::Normal;
    QString selectedZone_ = "chest";

    QRect headRect_;
    QRect chestRect_;
    QRect legsRect_;
};