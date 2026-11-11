#include "pages/AlertsPage.h"

#include <QLabel>
#include <QVBoxLayout>

AlertsPage::AlertsPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Alerts: TODO"));
    layout->addStretch();
}
