#include "pages/TrackPage.h"

#include <QDateTime>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

TrackPage::TrackPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // LEFT: Input Form Card
    auto* formCard = new QFrame;
    formCard->setObjectName("SpaceCard");
    formCard->setFixedWidth(360);
    auto* formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(18, 16, 18, 16);
    formLayout->setSpacing(12);

    auto* title = new QLabel("LOG BIOMETRIC READING");
    title->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700;");

    auto* subtitle = new QLabel("Measurements are saved offline and evaluated immediately against NASA-STD-3001 thresholds.");
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("color: #9ca3af; font-size: 11px;");

    auto* fields = new QFormLayout;
    fields->setSpacing(10);

    auto styleSpin = [](QWidget* w) {
        w->setStyleSheet("background-color: #070a13; color: #ffffff; border: 1px solid #1b2438; border-radius: 6px; padding: 6px;");
    };

    hrInput_ = new QSpinBox;
    hrInput_->setRange(30, 220);
    hrInput_->setValue(72);
    hrInput_->setSuffix(" BPM");
    styleSpin(hrInput_);

    sysInput_ = new QSpinBox;
    sysInput_->setRange(60, 240);
    sysInput_->setValue(120);
    sysInput_->setSuffix(" mmHg");
    styleSpin(sysInput_);

    diaInput_ = new QSpinBox;
    diaInput_->setRange(40, 150);
    diaInput_->setValue(80);
    diaInput_->setSuffix(" mmHg");
    styleSpin(diaInput_);

    sleepInput_ = new QDoubleSpinBox;
    sleepInput_->setRange(0.0, 24.0);
    sleepInput_->setSingleStep(0.5);
    sleepInput_->setValue(7.5);
    sleepInput_->setSuffix(" hrs");
    styleSpin(sleepInput_);

    moodInput_ = new QSpinBox;
    moodInput_->setRange(1, 5);
    moodInput_->setValue(4);
    moodInput_->setSuffix(" / 5");
    styleSpin(moodInput_);

    fields->addRow("Heart Rate:", hrInput_);
    fields->addRow("Systolic BP:", sysInput_);
    fields->addRow("Diastolic BP:", diaInput_);
    fields->addRow("Sleep Duration:", sleepInput_);
    fields->addRow("Mood Score:", moodInput_);

    auto* symLabel = new QLabel("REPORTED SYMPTOMS:");
    symLabel->setStyleSheet("color: #00d2ff; font-size: 11px; font-weight: 700; margin-top: 6px;");

    auto* symGrid = new QGridLayout;
    QStringList symptoms = {"Dizziness", "Headache", "Chest Pain", "Fatigue", "Vision Blur", "Nausea"};
    for (int i = 0; i < symptoms.size(); ++i) {
        auto* cb = new QCheckBox(symptoms[i]);
        cb->setStyleSheet("color: #d1d5db; font-size: 12px;");
        symptomChecks_.append(cb);
        symGrid->addWidget(cb, i / 2, i % 2);
    }

    auto* saveBtn = new QPushButton("Save Reading & Evaluate");
    saveBtn->setObjectName("PrimaryBtn");

    formLayout->addWidget(title);
    formLayout->addWidget(subtitle);
    formLayout->addLayout(fields);
    formLayout->addWidget(symLabel);
    formLayout->addLayout(symGrid);
    formLayout->addStretch();
    formLayout->addWidget(saveBtn);

    // RIGHT: History Table Card
    auto* tableCard = new QFrame;
    tableCard->setObjectName("SpaceCard");
    auto* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(18, 16, 18, 16);

    auto* tableTitle = new QLabel("MISSION VITALS HISTORY LOG");
    tableTitle->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700; margin-bottom: 8px;");

    historyTable_ = new QTableWidget(0, 6);
    historyTable_->setHorizontalHeaderLabels({"Timestamp (UTC)", "HR", "BP", "Sleep", "Mood", "Symptoms"});
    historyTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable_->verticalHeader()->setVisible(false);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable_->setStyleSheet(
        "QTableWidget { background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; gridline-color: #1b2438; color: #e5e7eb; }"
        "QHeaderView::section { background-color: #0d1322; color: #9ca3af; border: none; padding: 8px; font-weight: 700; font-size: 11px; }"
    );

    tableLayout->addWidget(tableTitle);
    tableLayout->addWidget(historyTable_, 1);

    root->addWidget(formCard);
    root->addWidget(tableCard, 1);

   
    connect(saveBtn, &QPushButton::clicked, this, &TrackPage::onSaveClicked);
}

void TrackPage::onSaveClicked() {
    astracare::VitalReading r;
    r.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
    r.heartRate = hrInput_->value();
    r.systolic = sysInput_->value();
    r.diastolic = diaInput_->value();
    r.sleepHours = sleepInput_->value();
    r.mood = moodInput_->value();

    QStringList selected;
    for (auto* cb : symptomChecks_) {
        if (cb->isChecked()) {
            selected << cb->text();
            cb->setChecked(false);
        }
    }
    r.symptoms = selected.join(", ").toStdString();

    appendRowToTable(r);
    emit readingLogged(r);
}

void TrackPage::appendRowToTable(const astracare::VitalReading& r) {
    historyTable_->insertRow(0);
    historyTable_->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(r.timestamp)));
    historyTable_->setItem(0, 1, new QTableWidgetItem(QString("%1 bpm").arg(r.heartRate)));
    historyTable_->setItem(0, 2, new QTableWidgetItem(QString("%1/%2").arg(r.systolic).arg(r.diastolic)));
    historyTable_->setItem(0, 3, new QTableWidgetItem(QString("%1 h").arg(r.sleepHours, 0, 'f', 1)));
    historyTable_->setItem(0, 4, new QTableWidgetItem(QString("%1 / 5").arg(r.mood)));
    historyTable_->setItem(0, 5, new QTableWidgetItem(r.symptoms.empty() ? "None" : QString::fromStdString(r.symptoms)));
}

void TrackPage::setReadings(const std::vector<astracare::VitalReading>& readings) {
    historyTable_->setRowCount(0);
    for (const auto& r : readings) {
        appendRowToTable(r);
    }
}
