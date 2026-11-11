#include "pages/GuidePage.h"

#include <QLabel>
#include <QVBoxLayout>

GuidePage::GuidePage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Medical guidance: TODO"));
    layout->addStretch();
}
