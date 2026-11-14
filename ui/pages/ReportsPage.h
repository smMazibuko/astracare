#pragma once
// Owner: Role 6 (Storage, Build and Reports)

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QWidget>

class ReportsPage : public QWidget {
    Q_OBJECT
public:
    explicit ReportsPage(QWidget* parent = nullptr);

public slots:
    void setReportData(const QString& jsonContent, const QString& csvContent);

signals:
    void generateReportRequested();

private slots:
    void exportJsonReport();
    void exportCsvReport();

private:
    QTextEdit* previewBox_;
    QLabel* statusLabel_;
    QPushButton* jsonBtn_;
    QPushButton* csvBtn_;

    QString jsonPayload_;
    QString csvPayload_;
};
