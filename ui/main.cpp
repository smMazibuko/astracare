#include <QApplication>
#include <QFile>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Astracare");

    // Load the space theme from the development branch
    QFile styleFile("../resources/styles/space_theme.qss");
    if (!styleFile.exists()) {
        styleFile.setFileName("resources/styles/space_theme.qss");
    }
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(styleFile.readAll());
    }

    // Load the Main Window (which has the navigation menu)
    MainWindow window;
    window.show();

    return app.exec();
}