#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include "src/mainwindow.h"
#include "src/ffi/rust.h"
#include "src/DockSystem/SplittableComponentsFactory.h"
#include <DockManager.h>
#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>

int main(int argc, char **argv) {

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);

    ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory);

    ffi::init();

    QCoreApplication::addLibraryPath(R"(C:\Qt\6.2.2\msvc2019_64\plugins)");
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
