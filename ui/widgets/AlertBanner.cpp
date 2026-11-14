#include "widgets/AlertBanner.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

AlertBanner::AlertBanner(QWidget* parent) : QFrame(parent) {
    setVisible(false);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(16, 12, 16, 12);
    mainLayout->setSpacing(12);

    auto* textCol = new QVBoxLayout;
    textCol->setSpacing(4);

    auto* topRow = new QHBoxLayout;
    badgeLabel_ = new QLabel("NORMAL");
    titleLabel_ = new QLabel("All systems nominal");
    titleLabel_->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700;");
    topRow->addWidget(badgeLabel_);
    topRow->addWidget(titleLabel_);
    topRow->addStretch();

    msgLabel_ = new QLabel;
    msgLabel_->setWordWrap(true);
    msgLabel_->setStyleSheet("color: #e5e7eb; font-size: 12px;");

    textCol->addLayout(topRow);
    textCol->addWidget(msgLabel_);

    guideBtn_ = new QPushButton("View Guidance →");
    guideBtn_->setObjectName("PrimaryBtn");

    sosBtn_ = new QPushButton("OPEN SOS");
    sosBtn_->setObjectName("SosBtn");

    mainLayout->addLayout(textCol, 1);
    mainLayout->addWidget(guideBtn_);
    mainLayout->addWidget(sosBtn_);

    connect(guideBtn_, &QPushButton::clicked, this, [this]() {
        emit viewGuidanceRequested(currentGuidanceId_);
    });
    connect(sosBtn_, &QPushButton::clicked, this, &AlertBanner::emergencyRequested);
}

void AlertBanner::setAlert(const astracare::Alert& alert) {
    if (alert.level == astracare::AlertLevel::Normal) {
        setVisible(false);
        return;
    }

    setVisible(true);
    currentGuidanceId_ = QString::fromStdString(alert.guidanceId);
    titleLabel_->setText(QString::fromStdString(alert.title));
    msgLabel_->setText(QString::fromStdString(alert.message));
    guideBtn_->setVisible(!currentGuidanceId_.isEmpty());

    if (alert.level == astracare::AlertLevel::Urgent) {
        setStyleSheet("QFrame { background-color: #3b0d11; border: 1px solid #ef4444; border-radius: 10px; }");
        badgeLabel_->setText("URGENT");
        badgeLabel_->setStyleSheet("background-color: #ef4444; color: #ffffff; padding: 2px 8px; border-radius: 4px; font-weight: 700; font-size: 11px;");
        sosBtn_->setVisible(true);
    } else {
        setStyleSheet("QFrame { background-color: #382309; border: 1px solid #f59e0b; border-radius: 10px; }");
        badgeLabel_->setText("CAUTION");
        badgeLabel_->setStyleSheet("background-color: #f59e0b; color: #070a13; padding: 2px 8px; border-radius: 4px; font-weight: 700; font-size: 11px;");
        sosBtn_->setVisible(false);
    }
}