#pragma once

#include <QFrame>
#include <QLabel>
#include <QString>

class VitalCard : public QFrame {
    Q_OBJECT
public:
    explicit VitalCard(const QString& title, const QString& unit, QWidget* parent = nullptr);

    void setReading(const QString& value, const QString& accentColorHex = "#10b981", const QString& statusText = "Nominal");

private:
    QLabel* titleLabel_;
    QLabel* valueLabel_;
    QLabel* unitLabel_;
    QLabel* statusLabel_;
};