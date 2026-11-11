#include "pages/EmergencyPage.h"

#include <QLabel>
#include <QVBoxLayout>

EmergencyPage::EmergencyPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Emergency procedures: TODO"));
    layout->addStretch();
}
