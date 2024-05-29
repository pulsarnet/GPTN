#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "ntdll.lib")

#define QT_MESSAGELOGCONTEXT

#include <QApplication>
#include <QFontDatabase>
#include <QSettings>
#include "MainWindow.h"
#include "DockSystem/SplittableComponentsFactory.h"
#include "Settings/RecentProjects.h"
#include <ptn/logger.h>
#include <DockManager.h>
#include <DockComponentsFactory.h>
#include "Core/ApplicationProjectController.h"
#include "Core/Graphviz/GraphvizWrapper.h"

void customMessageOutput(QtMsgType type, const QMessageLogContext& ctx, const QString &msg)
{
    namespace logger = ptn::logger;
    auto utf8LocalMsg = msg.toUtf8();
    const char* localMsg = utf8LocalMsg.constData();
    const char* function = ctx.function;
    const char* filename = ctx.file;
    int line = ctx.line;
    switch (type) {
        case QtDebugMsg:
            logger::debug(localMsg, function, filename, line);
            break;
        case QtInfoMsg:
            logger::info(localMsg, function, filename, line);
            break;
        case QtWarningMsg:
            logger::warn(localMsg, function, filename, line);
            break;
        case QtCriticalMsg:
            logger::error(localMsg, function, filename, line);
            break;
        case QtFatalMsg:
            logger::error(localMsg, function, filename, line);
            abort();
    }
}

int main(int argc, char **argv) {
    qInstallMessageHandler(customMessageOutput);

    QApplication::setApplicationDisplayName("GPTN");

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::HideSingleCentralWidgetTitleBar, true);

    ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory);

    ptn::logger::init();

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

    GraphVizWrapper wrapper;

    return app.exec();
}
