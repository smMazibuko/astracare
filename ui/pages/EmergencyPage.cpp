#include "pages/EmergencyPage.h"

#include <QCheckBox>
#include <QFrame>
#include <QPushButton>
#include <QStringList>

EmergencyPage::EmergencyPage(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // Compact emergency status card. Keep the warning prominent without
    // turning the entire content area into one oversized red strip.
    auto* sosBanner = new QFrame;
    sosBanner->setObjectName("EmergencyBanner");
    sosBanner->setMaximumWidth(620);
    auto* bannerLayout = new QHBoxLayout(sosBanner);
    bannerLayout->setContentsMargins(16, 12, 18, 12);
    bannerLayout->setSpacing(12);

    auto* sosBadge = new QLabel("SOS");
    sosBadge->setAlignment(Qt::AlignCenter);
    sosBadge->setFixedSize(48, 30);
    sosBadge->setStyleSheet("background-color: #ef4444; color: #ffffff; border-radius: 6px; font-size: 12px; font-weight: 800;");

    auto* bannerText = new QVBoxLayout;
    bannerText->setSpacing(2);
    auto* sosTitle = new QLabel("AUTONOMOUS EMERGENCY TRIAGE");
    sosTitle->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: 800;");
    auto* sosDetail = new QLabel("Earth comms delay: 4–24 min one-way");
    sosDetail->setStyleSheet("color: #fca5a5; font-size: 11px;");
    bannerText->addWidget(sosTitle);
    bannerText->addWidget(sosDetail);

    bannerLayout->addWidget(sosBadge);
    bannerLayout->addLayout(bannerText);
    root->addWidget(sosBanner, 0, Qt::AlignLeft);

    // Dynamic Protocol Selector Bar
    selectorLayout_ = new QHBoxLayout;
    selectorLayout_->setSpacing(10);
    root->addLayout(selectorLayout_);

    // Active Checklist Card
    auto* card = new QFrame;
    card->setObjectName("SpaceCard");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(20, 18, 20, 18);
    cardLayout->setSpacing(12);

    protocolTitle_ = new QLabel("No emergency protocols loaded");
    protocolTitle_->setStyleSheet("color: #fca5a5; font-size: 16px; font-weight: 700;");

    protocolSummary_ = new QLabel("Awaiting emergency procedure entries from core guidance loader.");
    protocolSummary_->setWordWrap(true);
    protocolSummary_->setStyleSheet("color: #9ca3af; font-size: 12px;");

    stepsLayout_ = new QVBoxLayout;
    stepsLayout_->setSpacing(10);

    cardLayout->addWidget(protocolTitle_);
    cardLayout->addWidget(protocolSummary_);
    cardLayout->addLayout(stepsLayout_);
    cardLayout->addStretch();

    root->addWidget(card, 1);
}

void EmergencyPage::setEmergencyProtocols(const std::vector<astracare::GuidanceEntry>& protocols) {
    protocols_ = protocols;

    // Clear selector buttons
    QLayoutItem* item;
    while ((item = selectorLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < static_cast<int>(protocols_.size()); ++i) {
        auto* btn = new QPushButton(QString::fromStdString(protocols_[i].title));
        btn->setObjectName("SosBtn");
        connect(btn, &QPushButton::clicked, this, [this, i]() { displayProtocol(i); });
        selectorLayout_->addWidget(btn);
    }

    if (!protocols_.empty()) {
        displayProtocol(0);
    } else {
        protocolTitle_->setText("No emergency protocols loaded");
        protocolSummary_->setText("Awaiting emergency procedure entries from core guidance loader.");
    }
}

void EmergencyPage::selectProtocolById(const QString& guidanceId) {
    for (int i = 0; i < static_cast<int>(protocols_.size()); ++i) {
        if (QString::fromStdString(protocols_[i].id) == guidanceId) {
            displayProtocol(i);
            return;
        }
    }
}

void EmergencyPage::displayProtocol(int index) {
    if (index < 0 || index >= static_cast<int>(protocols_.size())) return;

    // Clear existing checklist items
    QLayoutItem* item;
    while ((item = stepsLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    const auto& proto = protocols_[index];
    protocolTitle_->setText(QString::fromStdString(proto.title).toUpper());
    protocolSummary_->setText(QString::fromStdString(proto.summary));

    QStringList lines = QString::fromStdString(proto.steps).split('\n', Qt::SkipEmptyParts);
    for (const QString& step : lines) {
        auto* cb = new QCheckBox(step.trimmed());
        cb->setStyleSheet(
            "QCheckBox { background-color: rgba(0, 0, 0, 60); border: 1px solid rgba(255, 255, 255, 38); border-radius: 8px; padding: 12px; color: #f3f4f6; font-size: 13px; }"
            "QCheckBox:checked { border-color: #10b981; color: #9ca3af; }"
        );
        stepsLayout_->addWidget(cb);
    }
}
