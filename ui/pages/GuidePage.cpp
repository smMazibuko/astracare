#include "pages/GuidePage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

GuidePage::GuidePage(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // LEFT: Search & Procedure List
    auto* leftCard = new QFrame;
    leftCard->setObjectName("SpaceCard");
    leftCard->setFixedWidth(340);
    auto* leftLayout = new QVBoxLayout(leftCard);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(10);

    auto* listTitle = new QLabel("OFFLINE MEDICAL LIBRARY");
    listTitle->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700;");

    searchBox_ = new QLineEdit;
    searchBox_->setPlaceholderText("Search medical procedures...");
    searchBox_->setStyleSheet("background-color: rgba(0, 0, 0, 80); color: #ffffff; border: 1px solid rgba(255, 255, 255, 42); border-radius: 8px; padding: 8px 12px;");

    guideList_ = new QListWidget;
    guideList_->setStyleSheet(
        "QListWidget { background-color: rgba(0, 0, 0, 60); border: 1px solid rgba(255, 255, 255, 38); border-radius: 8px; outline: none; }"
        "QListWidget::item { color: #d1d5db; padding: 12px; border-bottom: 1px solid rgba(255, 255, 255, 24); }"
        "QListWidget::item:selected { background-color: rgba(47, 143, 91, 38); color: #2f8f5b; font-weight: 700; }"
    );

    leftLayout->addWidget(listTitle);
    leftLayout->addWidget(searchBox_);
    leftLayout->addWidget(guideList_, 1);

    // RIGHT: Procedure Detail Reader
    auto* rightCard = new QFrame;
    rightCard->setObjectName("SpaceCard");
    auto* rightLayout = new QVBoxLayout(rightCard);
    rightLayout->setContentsMargins(20, 18, 20, 18);
    rightLayout->setSpacing(12);

    categoryBadge_ = new QLabel("GUIDANCE LIBRARY");
    categoryBadge_->setStyleSheet("color: #2f8f5b; font-family: 'Consolas', monospace; font-size: 11px; font-weight: 700;");

    titleLabel_ = new QLabel("No guidance entries loaded");
    titleLabel_->setWordWrap(true);
    titleLabel_->setStyleSheet("color: #ffffff; font-size: 18px; font-weight: 700;");

    contentBrowser_ = new QTextBrowser;
    contentBrowser_->setStyleSheet("background-color: rgba(0, 0, 0, 60); border: 1px solid rgba(255, 255, 255, 38); border-radius: 8px; padding: 14px; color: #e5e7eb; font-size: 13px;");
    contentBrowser_->setPlainText("Awaiting offline guidance entries from core guidance loader...");

    rightLayout->addWidget(categoryBadge_);
    rightLayout->addWidget(titleLabel_);
    rightLayout->addWidget(contentBrowser_, 1);

    root->addWidget(leftCard);
    root->addWidget(rightCard, 1);

    connect(searchBox_, &QLineEdit::textChanged, this, &GuidePage::filterGuides);
    connect(guideList_, &QListWidget::currentRowChanged, this, &GuidePage::onGuideSelected);
}

void GuidePage::setGuidanceEntries(const std::vector<astracare::GuidanceEntry>& entries) {
    entries_ = entries;
    filterGuides(searchBox_->text());
}

void GuidePage::filterGuides(const QString& query) {
    guideList_->clear();
    filteredEntries_.clear();
    QString q = query.trimmed().toLower();

    for (const auto& entry : entries_) {
        QString title = QString::fromStdString(entry.title);
        QString summary = QString::fromStdString(entry.summary);
        QString cat = QString::fromStdString(entry.category);

        if (q.isEmpty() || title.toLower().contains(q) || summary.toLower().contains(q) || cat.toLower().contains(q)) {
            filteredEntries_.push_back(entry);
            guideList_->addItem(title);
        }
    }

    if (!filteredEntries_.empty()) {
        guideList_->setCurrentRow(0);
    } else {
        categoryBadge_->setText("GUIDANCE LIBRARY");
        titleLabel_->setText(entries_.empty() ? "No guidance entries loaded" : "No matching procedures");
        contentBrowser_->clear();
    }
}

void GuidePage::onGuideSelected(int row) {
    if (row < 0 || row >= static_cast<int>(filteredEntries_.size())) return;
    const auto& g = filteredEntries_[row];

    categoryBadge_->setText(QString::fromStdString(g.category).toUpper());
    titleLabel_->setText(QString::fromStdString(g.title));

    QString stepsHtml = QString::fromStdString(g.steps).replace("\n", "<br><br>");
    QString html = QString(
        "<p style='color:#9ca3af;'><b>Summary:</b> %1</p>"
        "<hr style='border:1px solid #273029; margin:12px 0;'>"
        "<p style='color:#2f8f5b; font-weight:bold;'>PROCEDURE STEPS:</p>"
        "<p style='line-height:1.6;'>%2</p>"
        "<hr style='border:1px solid #273029; margin:12px 0;'>"
        "<p style='color:#6b7280; font-size:11px;'>Source: %3</p>"
    ).arg(QString::fromStdString(g.summary), stepsHtml, QString::fromStdString(g.source));

    contentBrowser_->setHtml(html);
}

void GuidePage::selectGuidanceById(const QString& guidanceId) {
    searchBox_->clear();
    for (int i = 0; i < static_cast<int>(filteredEntries_.size()); ++i) {
        if (QString::fromStdString(filteredEntries_[i].id) == guidanceId) {
            guideList_->setCurrentRow(i);
            return;
        }
    }
}
