#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QSettings>
#include "src/mainwindow.h"
#include "src/ffi/rust.h"
#include "src/DockSystem/SplittableComponentsFactory.h"
#include <DockManager.h>
#include <DockComponentsFactory.h>

int main(int argc, char **argv) {

    QCoreApplication::setOrganizationName("GPTN");
    QCoreApplication::setOrganizationDomain("lamas.tech");
    QCoreApplication::setApplicationName("GPTN");

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);

    ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory);

    // Если NDEBUG, тогда включаем логирование
    #ifdef QT_DEBUG
        ffi::init();
    #endif

    QApplication app(argc, argv);
    auto palette = QApplication::palette();
    palette.setColor(QPalette::All, QPalette::Base, Qt::white);

    // Settings INI
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);

    MainWindow window(nullptr);
    window.setMinimumSize(QSize(1280, 720));
    window.show();

    return app.exec();
}
