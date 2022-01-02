#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#include <QApplication>
#include "include/mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);

//    app.setStyleSheet(R"(
//        QMainWindow {
//            background-color: #24292E;
//        }

//        QMainWindow::separator {
//            width: 1px;
//        }

//        QDockWidget {
//            padding: 0;
//            border: 1px solid green;
//        }

//        QDockWidget::title {
//            text-align: left;
//            background-color: #1F2428;
//            border-bottom: 1px solid white;
//            padding-left: 8px;
//        }

//        QDockWidget::close-button:hover, QDockWidget::float-button:hover {
//            background-color: #88544C;
//        }
//    )");

    MainWindow window(nullptr);
    window.setMinimumWidth(800);
    window.setMinimumHeight(600);
    window.show();

    return app.exec();
}
