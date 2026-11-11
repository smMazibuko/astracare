#include "pages/TrackPage.h"

#include <QLabel>
#include <QVBoxLayout>

TrackPage::TrackPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Track vitals: TODO"));
    layout->addStretch();
}
