
#include <QApplication>
#include "mainwindow.h"

extern "C" void update_i32(int* a);

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow window(nullptr);
    window.show();

    return app.exec();
}
