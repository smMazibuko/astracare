#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Astracare");

    QFile styleFile("../resources/styles/space_theme.qss");
    if (!styleFile.exists()) {
        styleFile.setFileName("resources/styles/space_theme.qss");
    }
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(styleFile.readAll());
    }
    
    MainWindow window;
    window.show();

    return app.exec();
}
