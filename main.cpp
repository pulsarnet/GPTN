#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include "src/mainwindow.h"

int main(int argc, char **argv) {
    QCoreApplication::addLibraryPath("C:\\Qt\\6.2.2\\msvc2019_64\\plugins");
    QApplication app(argc, argv);

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);

    QFile fontRes(":/fonts/JetBrainsMono-Medium.ttf");
    if (!fontRes.open(QIODevice::ReadOnly)) {
        qDebug() << fontRes.errorString();
        return -1;
    }

    auto id = QFontDatabase::addApplicationFontFromData(fontRes.readAll());
    qDebug() << id;
    QApplication::setFont(QFont("JetBrains Mono", 13, QFont::Medium));

    MainWindow window(nullptr);
    window.setWindowState(Qt::WindowMaximized);
    window.show();

    return app.exec();
}
