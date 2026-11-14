#pragma once
// Owner: Role 5 (Medical Content)

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QTextBrowser>
#include <QWidget>
#include <vector>
#include "astracare/models.h"

class GuidePage : public QWidget {
    Q_OBJECT
public:
    explicit GuidePage(QWidget* parent = nullptr);

    void selectGuidanceById(const QString& guidanceId);

public slots:
    void setGuidanceEntries(const std::vector<astracare::GuidanceEntry>& entries);
    void selectGuidanceById(const QString& guidanceId);
    
private slots:
    void filterGuides(const QString& query);
    void onGuideSelected(int row);

private:
    QLineEdit* searchBox_;
    QListWidget* guideList_;
    QLabel* titleLabel_;
    QLabel* categoryBadge_;
    QTextBrowser* contentBrowser_;

    std::vector<astracare::GuidanceEntry> entries_;
    std::vector<astracare::GuidanceEntry> filteredEntries_;
};
