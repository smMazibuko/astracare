#include "pages/ReportsPage.h"

#include <QLabel>
#include <QVBoxLayout>

ReportsPage::ReportsPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Reports: TODO"));
    layout->addStretch();
}
