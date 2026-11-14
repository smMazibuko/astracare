#pragma once
// Owner: Role 5 (Medical Content)

#include <QCheckBox>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>
#include <QWidget>

class EmergencyPage : public QWidget {
    Q_OBJECT
public:
    explicit EmergencyPage(QWidget* parent = nullptr);

public slots:
    void setEmergencyProtocols(const std::vector<astracare::GuidanceEntry>& protocols);
    void selectProtocolById(const QString& guidanceId);

private slots:
    void displayProtocol(int index);

private:
    QHBoxLayout* selectorLayout_;
    QLabel* protocolTitle_;
    QLabel* protocolSummary_;
    QVBoxLayout* stepsLayout_;

    std::vector<astracare::GuidanceEntry> protocols_;
};