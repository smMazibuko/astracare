#include "widgets/VitalCard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

VitalCard::VitalCard(const QString& title, const QString& unit, QWidget* parent)
    : QFrame(parent) {
    setObjectName("MetricTile");
    setMinimumHeight(92);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);

    titleLabel_ = new QLabel(title.toUpper());
    titleLabel_->setStyleSheet("color: #9ca3af; font-size: 11px; font-weight: 600; letter-spacing: 0.5px;");

    auto* valRow = new QHBoxLayout;
    valRow->setSpacing(6);

    valueLabel_ = new QLabel("--");
    valueLabel_->setStyleSheet("color: #10b981; font-size: 24px; font-weight: 700; font-family: 'Consolas', monospace;");

    unitLabel_ = new QLabel(unit);
    unitLabel_->setStyleSheet("color: #6b7280; font-size: 12px; padding-top: 6px;");

    valRow->addWidget(valueLabel_);
    valRow->addWidget(unitLabel_);
    valRow->addStretch();

    statusLabel_ = new QLabel("Nominal");
    statusLabel_->setStyleSheet("color: #10b981; font-size: 11px;");

    layout->addWidget(titleLabel_);
    layout->addLayout(valRow);
    layout->addWidget(statusLabel_);
}

void VitalCard::setReading(const QString& value, const QString& accentColorHex, const QString& statusText) {
    valueLabel_->setText(value);
    valueLabel_->setStyleSheet(
        QString("color: %1; font-size: 24px; font-weight: 700; font-family: 'Consolas', monospace;")
            .arg(accentColorHex));
    statusLabel_->setText(statusText);
    statusLabel_->setStyleSheet(QString("color: %1; font-size: 11px;").arg(accentColorHex));
}