#include <QApplication>

#include "MainWindow.h"

#include "pages/TrackPage.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Astracare");

    //MainWindow window;
    //window.show();

    TrackPage page;
    page.show();

    return app.exec();
}
