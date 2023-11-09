#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "ntdll.lib")

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QSettings>
#include "src/MainWindow.h"
#include "src/DockSystem/SplittableComponentsFactory.h"
#include "src/Settings/RecentProjects.h"
#include "src/Core/FFI/rust.h"
#include <DockManager.h>
#include <DockComponentsFactory.h>
#include "src/Core/ApplicationProjectController.h"

#define QT_MESSAGELOGCONTEXT

extern "C" void external_debug_log(const char* msg);
extern "C" void external_info_log(const char* msg);
extern "C" void external_warn_log(const char* msg);
extern "C" void external_error_log(const char* msg);

void customMessageOutput(QtMsgType type, const QMessageLogContext&, const QString &msg)
{
    const char* localMsg = msg.toLocal8Bit().data();
    switch (type) {
        case QtDebugMsg:
            external_debug_log(localMsg);
            break;
        case QtInfoMsg:
            external_info_log(localMsg);
            break;
        case QtWarningMsg:
            external_warn_log(localMsg);
            break;
        case QtCriticalMsg:
            external_error_log(localMsg);
            break;
        case QtFatalMsg:
            external_error_log(localMsg);
            abort();
    }
}

int main(int argc, char **argv) {
    qInstallMessageHandler(customMessageOutput);

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::HideSingleCentralWidgetTitleBar, true);

    ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory);

    //#ifdef QT_DEBUG
        ffi::init();
    //#endif

    QApplication app(argc, argv);
    auto palette = QApplication::palette();
    // Light theme
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(0xf0, 0xf0, 0xf0));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0x42, 0x85, 0xf4));
    palette.setColor(QPalette::Highlight, QColor(0x42, 0x85, 0xf4));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Window, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Dark, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Mid, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(0x80, 0x80, 0x80));
    QApplication::setPalette(palette);

    // Settings INI
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);

    // init
    if (!RecentProjects::init()) {
        QMessageBox::critical(nullptr, "Error", "Can't init recent projects");
        return 1;
    }

    ApplicationProjectController controller;
    controller.emptyProject();

    return app.exec();
}
