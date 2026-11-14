#include "pages/AlertsPage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include "widgets/AlertBanner.h"

AlertsPage::AlertsPage(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // LEFT: Active Alerts Feed
    auto* feedCard = new QFrame;
    feedCard->setObjectName("SpaceCard");
    auto* feedLayout = new QVBoxLayout(feedCard);
    feedLayout->setContentsMargins(18, 16, 18, 16);

    auto* title = new QLabel("ACTIVE & RECENT HEALTH ALERTS");
    title->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700; margin-bottom: 8px;");

    emptyStateLabel_ = new QLabel("No active health alerts. All evaluated readings are within normal parameters.");
    emptyStateLabel_->setWordWrap(true);
    emptyStateLabel_->setStyleSheet(
        "background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; "
        "padding: 16px; color: #9ca3af; font-size: 12px;"
    );

    alertsListLayout_ = new QVBoxLayout;
    alertsListLayout_->setSpacing(10);

    feedLayout->addWidget(title);
    feedLayout->addWidget(emptyStateLabel_);
    feedLayout->addLayout(alertsListLayout_);
    feedLayout->addStretch();

    // RIGHT: Configured Reference Ranges Table (populated by backend)
    auto* rangeCard = new QFrame;
    rangeCard->setObjectName("SpaceCard");
    rangeCard->setFixedWidth(400);
    auto* rangeLayout = new QVBoxLayout(rangeCard);
    rangeLayout->setContentsMargins(18, 16, 18, 16);

    auto* rangeTitle = new QLabel("CONFIGURED THRESHOLD RANGES");
    rangeTitle->setStyleSheet("color: #00d2ff; font-size: 13px; font-weight: 700; margin-bottom: 8px;");

    rangeTable_ = new QTableWidget(0, 3);
    rangeTable_->setHorizontalHeaderLabels({"Biometric", "Normal Range", "Alert Trigger"});
    rangeTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rangeTable_->verticalHeader()->setVisible(false);
    rangeTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rangeTable_->setStyleSheet(
        "QTableWidget { background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; color: #e5e7eb; font-size: 12px; }"
        "QHeaderView::section { background-color: #0d1322; color: #9ca3af; border: none; padding: 8px; font-weight: 700; }"
    );

    rangeLayout->addWidget(rangeTitle);
    rangeLayout->addWidget(rangeTable_, 1);

    root->addWidget(feedCard, 1);
    root->addWidget(rangeCard);
}

void AlertsPage::setAlerts(const std::vector<astracare::Alert>& alerts) {
    QLayoutItem* item;
    while ((item = alertsListLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    bool hasVisibleAlert = false;
    for (const auto& alert : alerts) {
        if (alert.level != astracare::AlertLevel::Normal) {
            addTriggeredAlert(alert);
            hasVisibleAlert = true;
        }
    }
    emptyStateLabel_->setVisible(!hasVisibleAlert);
}

void AlertsPage::addTriggeredAlert(const astracare::Alert& alert) {
    if (alert.level == astracare::AlertLevel::Normal) return;

    emptyStateLabel_->setVisible(false);
    auto* banner = new AlertBanner;
    banner->setAlert(alert);
    connect(banner, &AlertBanner::viewGuidanceRequested, this, &AlertsPage::openGuidanceRequested);
    connect(banner, &AlertBanner::emergencyRequested, this, &AlertsPage::emergencyRequested);
    alertsListLayout_->insertWidget(0, banner);
}

void AlertsPage::setThresholds(const std::vector<std::array<QString, 3>>& rows) {
    rangeTable_->setRowCount(static_cast<int>(rows.size()));
    for (int r = 0; r < static_cast<int>(rows.size()); ++r) {
        for (int c = 0; c < 3; ++c) {
            rangeTable_->setItem(r, c, new QTableWidgetItem(rows[r][c]));
        }
    }
}