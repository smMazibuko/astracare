#include "pages/ReportsPage.h"

#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QPageSize>
#include <QPdfWriter>
#include <QTextStream>
#include <QVBoxLayout>

ReportsPage::ReportsPage(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
   root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // Top Control Bar
    auto* topCard = new QFrame;
    topCard->setObjectName("SpaceCard");
    auto* topLayout = new QHBoxLayout(topCard);
    topLayout->setContentsMargins(18, 16, 18, 16);

    auto* infoCol = new QVBoxLayout;
    auto* title = new QLabel("EARTH FLIGHT SURGEON REPORT & DTN TRANSMISSION PACKET");
    title->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: 700;");
    auto* desc = new QLabel("Compiles offline vitals history, active alerts, and accessed medical guidance for transmission when Earth comms are available.");
    desc->setStyleSheet("color: #9ca3af; font-size: 12px;");
    infoCol->addWidget(title);
    infoCol->addWidget(desc);

    auto* refreshBtn = new QPushButton("Compile Latest Report");
    jsonBtn_ = new QPushButton("Export JSON");
    jsonBtn_->setObjectName("PrimaryBtn");
    jsonBtn_->setEnabled(false);

    csvBtn_ = new QPushButton("Export CSV");
    csvBtn_->setEnabled(false);

    pdfBtn_ = new QPushButton("Export PDF");
    pdfBtn_->setEnabled(false);

    topLayout->addLayout(infoCol, 1);
    topLayout->addWidget(refreshBtn);
    topLayout->addWidget(jsonBtn_);
    topLayout->addWidget(csvBtn_);
    topLayout->addWidget(pdfBtn_);

    // Report Preview Box
    auto* previewCard = new QFrame;
    previewCard->setObjectName("SpaceCard");
    auto* previewLayout = new QVBoxLayout(previewCard);
    previewLayout->setContentsMargins(18, 16, 18, 16);

    auto* prevLabel = new QLabel("TELEMETRY BUNDLE PREVIEW");
    prevLabel->setStyleSheet("color: #2f8f5b; font-size: 11px; font-weight: 700; margin-bottom: 6px;");

    previewBox_ = new QTextEdit;
    previewBox_->setReadOnly(true);
    previewBox_->setPlaceholderText("No report compiled yet. Click 'Compile Latest Report' or await backend report generation.");
    previewBox_->setStyleSheet(
        "background-color: #070a13; border: 1px solid #1b2438; border-radius: 8px; "
        "padding: 12px; color: #10b981; font-family: 'Consolas', monospace; font-size: 12px;"
    );

    statusLabel_ = new QLabel("Awaiting report payload from storage/report module.");
    statusLabel_->setStyleSheet("color: #9ca3af; font-size: 11px; margin-top: 6px;");

    previewLayout->addWidget(prevLabel);
    previewLayout->addWidget(previewBox_, 1);
    previewLayout->addWidget(statusLabel_);

    root->addWidget(topCard);
    root->addWidget(previewCard, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &ReportsPage::generateReportRequested);
    connect(jsonBtn_, &QPushButton::clicked, this, &ReportsPage::exportJsonReport);
    connect(csvBtn_, &QPushButton::clicked, this, &ReportsPage::exportCsvReport);
    connect(pdfBtn_, &QPushButton::clicked, this, &ReportsPage::exportPdfReport);
}

void ReportsPage::setReportData(const QString& jsonContent, const QString& csvContent) {
    jsonPayload_ = jsonContent;
    csvPayload_ = csvContent;

    previewBox_->setPlainText(jsonPayload_);
    jsonBtn_->setEnabled(!jsonPayload_.isEmpty());
    csvBtn_->setEnabled(!csvPayload_.isEmpty());
    pdfBtn_->setEnabled(!jsonPayload_.isEmpty());
    statusLabel_->setText("Report bundle ready for offline export.");
    statusLabel_->setStyleSheet("color: #2f8f5b; font-size: 11px; margin-top: 6px;");
}

void ReportsPage::exportPdfReport() {
    if (jsonPayload_.isEmpty()) return;
    const QString path = QFileDialog::getSaveFileName(this, "Save PDF Medical Report",
                                                       "astracare_report.pdf", "PDF Files (*.pdf)");
    if (path.isEmpty()) return;

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setTitle("Astracare Medical Report");
    QPainter painter(&writer);
    painter.setFont(QFont("Arial", 10));
    const int margin = 80;
    const int lineHeight = 18;
    int y = margin;
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(margin, y, "ASTRACARE MEDICAL REPORT");
    y += lineHeight * 2;
    painter.setFont(QFont("Consolas", 9));
    for (const QString& line : jsonPayload_.split('\n')) {
        if (y > writer.height() - margin) {
            writer.newPage();
            y = margin;
        }
        painter.drawText(margin, y, line.left(150));
        y += lineHeight;
    }
    painter.end();
    statusLabel_->setText("Exported PDF report to: " + path);
    statusLabel_->setStyleSheet("color: #10b981; font-size: 11px; font-weight: 700; margin-top: 6px;");
}

void ReportsPage::exportJsonReport() {
    if (jsonPayload_.isEmpty()) return;
    QString path = QFileDialog::getSaveFileName(this, "Save JSON Medical Report", "astracare_report.json", "JSON Files (*.json)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << jsonPayload_;
        statusLabel_->setText("Exported JSON report to: " + path);
        statusLabel_->setStyleSheet("color: #10b981; font-size: 11px; font-weight: 700; margin-top: 6px;");
    }
}

void ReportsPage::exportCsvReport() {
    if (csvPayload_.isEmpty()) return;
    QString path = QFileDialog::getSaveFileName(this, "Save CSV Vitals Log", "astracare_vitals.csv", "CSV Files (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << csvPayload_;
        statusLabel_->setText("Exported CSV log to: " + path);
        statusLabel_->setStyleSheet("color: #10b981; font-size: 11px; font-weight: 700; margin-top: 6px;");
    }
}
