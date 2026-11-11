#include "pages/DashboardPage.h"

#include <QLabel>
#include <QVBoxLayout>

DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Dashboard: TODO"));
    layout->addStretch();
}
