#include "widgets/BodyTwinWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

BodyTwinWidget::BodyTwinWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(210, 320);
    setCursor(Qt::PointingHandCursor);
}

void BodyTwinWidget::setZoneStatuses(astracare::AlertLevel head, astracare::AlertLevel chest, astracare::AlertLevel legs) {
    headLevel_ = head;
    chestLevel_ = chest;
    legsLevel_ = legs;
    update();
}

void BodyTwinWidget::setSelectedZone(const QString& zone) {
    selectedZone_ = zone;
    update();
}

QColor BodyTwinWidget::colorForLevel(astracare::AlertLevel level) const {
    if (level == astracare::AlertLevel::Urgent) return QColor("#ef4444");
    if (level == astracare::AlertLevel::Caution) return QColor("#f59e0b");
    return QColor("#10b981");
}

void BodyTwinWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int cx = width() / 2;
    headRect_ = QRect(cx - 28, 16, 56, 56);
    chestRect_ = QRect(cx - 42, 82, 84, 92);
    legsRect_ = QRect(cx - 38, 184, 76, 122);

    auto drawZone = [&](const QRect& rect, astracare::AlertLevel lvl, const QString& id, const QString& label, bool isCircle = false) {
        QColor base = colorForLevel(lvl);
        QColor fill = base;
        fill.setAlpha(selectedZone_ == id ? 115 : 48);

        QPen pen(selectedZone_ == id ? QColor("#00d2ff") : base, selectedZone_ == id ? 3 : 2);
        p.setPen(pen);
        p.setBrush(fill);

        if (isCircle) {
            p.drawEllipse(rect);
        } else {
            p.drawRoundedRect(rect, 12, 12);
        }

        p.setPen(Qt::white);
        QFont f = p.font();
        f.setPointSize(8);
        f.setBold(true);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, label);
    };

    // Arms (decorative silhouette)
    p.setPen(QPen(QColor("#334155"), 1.5));
    p.setBrush(QColor("#1b2438"));
    p.drawRoundedRect(cx - 68, 86, 20, 82, 9, 9);
    p.drawRoundedRect(cx + 48, 86, 20, 82, 9, 9);

    drawZone(headRect_, headLevel_, "head", "HEAD\nSANS / CO2", true);
    drawZone(chestRect_, chestLevel_, "chest", "CARDIAC\nHR / BP", false);
    drawZone(legsRect_, legsLevel_, "legs", "LOWER BODY\nFLUID / BONE", false);
}

void BodyTwinWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    if (headRect_.contains(pos)) {
        selectedZone_ = "head";
        emit zoneSelected("head");
        update();
    } else if (chestRect_.contains(pos)) {
        selectedZone_ = "chest";
        emit zoneSelected("chest");
        update();
    } else if (legsRect_.contains(pos)) {
        selectedZone_ = "legs";
        emit zoneSelected("legs");
        update();
    }
}