#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include "src/mainwindow.h"

int main(int argc, char **argv) {
    QCoreApplication::addLibraryPath("C:\\Qt\\6.2.3\\msvc2019_64\\plugins");
    QApplication app(argc, argv);

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);
    QApplication::setFont(QFont("JetBrains Mono", 13, QFont::Medium));

    MainWindow window(nullptr);
    window.setWindowState(Qt::WindowMaximized);
    window.show();

    return app.exec();
}
