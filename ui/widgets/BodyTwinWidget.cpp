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

        QPen pen(selectedZone_ == id ? QColor("#2f8f5b") : base, selectedZone_ == id ? 3 : 2);
        p.setPen(pen);
        p.setBrush(fill);

        if (isCircle) {
            p.drawEllipse(rect);
        } else {
            p.drawRoundedRect(rect, 12, 12);
        }

        // Draw each line independently so the label remains optically centered
        // instead of relying on the default multi-line text leading.
        p.setPen(Qt::white);
        QFont f = p.font();
        f.setPointSizeF(isCircle ? 7.5 : 8.0);
        f.setBold(true);

        const QStringList lines = label.split('\n');
        const qreal horizontalPadding = isCircle ? 8.0 : 10.0;
        const qreal availableWidth = rect.width() - horizontalPadding;
        QFontMetricsF metrics(f);
        while (metrics.horizontalAdvance(lines.value(0)) > availableWidth ||
               (lines.size() > 1 && metrics.horizontalAdvance(lines.at(1)) > availableWidth)) {
            f.setPointSizeF(f.pointSizeF() - 0.5);
            metrics = QFontMetricsF(f);
            if (f.pointSizeF() <= 6.0) break;
        }

        p.setFont(f);
        const qreal lineHeight = metrics.lineSpacing();
        const qreal totalHeight = lineHeight * lines.size();
        qreal baseline = rect.center().y() - (totalHeight / 2.0) + metrics.ascent();
        for (const QString& line : lines) {
            const QRectF lineRect(rect.left() + horizontalPadding / 2.0,
                                  baseline - metrics.ascent(),
                                  availableWidth,
                                  lineHeight);
            p.drawText(lineRect, Qt::AlignHCenter | Qt::AlignVCenter, line);
            baseline += lineHeight;
        }
    };

    // Arms (decorative silhouette)
    p.setPen(QPen(QColor("#334155"), 1.5));
    p.setBrush(QColor("#273029"));
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
