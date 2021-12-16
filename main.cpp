#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#include <QApplication>
#include "include/mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow window(nullptr);
    window.setMinimumWidth(800);
    window.setMinimumHeight(600);
    window.show();

    return app.exec();
}
