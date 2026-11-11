#pragma once
// Owner: Role 2 (UI Lead)

#include <QMainWindow>

class QListWidget;
class QStackedWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QListWidget* nav_ = nullptr;
    QStackedWidget* pages_ = nullptr;
};
