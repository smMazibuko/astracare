#include "TrackPage.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include "astracare/models.h" 

TrackPage::TrackPage(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Astronaut Vitals Tracking");
    title->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px; color: white;");
    mainLayout->addWidget(title);

    QFormLayout* formLayout = new QFormLayout();

    // 1. Heart Rate
    QSpinBox* hrInput = new QSpinBox();
    hrInput->setRange(0, 300);
    hrInput->setSuffix(" bpm");
    formLayout->addRow("Heart Rate:", hrInput);

    // 2. Blood Pressure
    QWidget* bpWidget = new QWidget();
    QHBoxLayout* bpLayout = new QHBoxLayout(bpWidget);
    bpLayout->setContentsMargins(0, 0, 0, 0);

    QSpinBox* sysInput = new QSpinBox();
    sysInput->setRange(0, 300);
    sysInput->setPrefix("Sys: ");

    QSpinBox* diaInput = new QSpinBox();
    diaInput->setRange(0, 200);
    diaInput->setPrefix("Dia: ");

    bpLayout->addWidget(sysInput);
    bpLayout->addWidget(diaInput);
    formLayout->addRow("Blood Pressure:", bpWidget);

    // 3. Sleep
    QDoubleSpinBox* sleepInput = new QDoubleSpinBox();
    sleepInput->setRange(0.0, 24.0);
    sleepInput->setSingleStep(0.5);
    sleepInput->setSuffix(" hours");
    formLayout->addRow("Sleep:", sleepInput);

    // 4. Mood (1 to 5)
    QSpinBox* moodInput = new QSpinBox();
    moodInput->setRange(1, 5);
    moodInput->setPrefix("Level 1-5: ");
    formLayout->addRow("Mood:", moodInput);

    // 5. Symptoms
    QTextEdit* symptomsInput = new QTextEdit();
    symptomsInput->setPlaceholderText("Enter any symptoms...");
    symptomsInput->setMaximumHeight(80);
    formLayout->addRow("Symptoms:", symptomsInput);

    mainLayout->addLayout(formLayout);

    // Save Button
    QPushButton* saveBtn = new QPushButton("Save Reading");
    saveBtn->setStyleSheet(
        "background-color: #2b579a; color: white; padding: 10px; font-weight: bold;");
    mainLayout->addWidget(saveBtn);

    mainLayout->addStretch();

        // Button Click Action - BACKEND LOGIC
    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        // 1. Validation: Check for impossible medical data
        if (sysInput->value() <= diaInput->value()) {
            QMessageBox::warning(this, "Invalid Data",
                                 "Systolic BP must be higher than Diastolic BP.");
            return; // Stop the save
        }
        if (hrInput->value() < 30 || hrInput->value() > 220) {
            QMessageBox::warning(this, "Invalid Data",
                                 "Heart rate is outside of human survival limits.");
            return;
        }

        // 2. Create the backend data struct
        VitalReading reading;
        reading.id = 0; // Will be assigned by the database
        // In a real app, use a proper date library for ISO 8601. For now, a placeholder:
        reading.timestamp = "2026-11-14T12:00:00";
        reading.heartRate = hrInput->value();
        reading.systolic = sysInput->value();
        reading.diastolic = diaInput->value();
        reading.sleepHours = sleepInput->value();
        reading.mood = moodInput->value();
        reading.symptoms = symptomsInput->toPlainText().toStdString();

        // 3. TODO: Pass to Database (@Madiki)
        // Database::getInstance().saveVitalReading(reading);

        // 4. TODO: Pass to Alert Engine (@Sunday)
        // std::vector<Alert> alerts = AlertEngine::analyze(reading);
        // if (!alerts.empty()) { show alerts on dashboard }

        // Temporary confirmation so you know the backend worked
        QString msg = QString("Backend Success!\nHR: %1\nBP: %2/%3\nSleep: %4\nMood: %5")
                          .arg(reading.heartRate)
                          .arg(reading.systolic)
                          .arg(reading.diastolic)
                          .arg(reading.sleepHours)
                          .arg(reading.mood);

        QMessageBox::information(this, "Reading Processed", msg);
    });
}