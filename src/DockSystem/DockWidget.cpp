//
// Created by nmuravev on 3/21/2022.
//

#include <QBoxLayout>
#include "DockWidget.h"
#include "DockToolbar.h"
#include <DockAreaWidget.h>


DockWidget::DockWidget(const QString& name, QWidget *parent) : ads::CDockWidget(name, parent) {

    setFrameShape(QFrame::NoFrame);

    setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    //setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    setFeature(ads::CDockWidget::NoTab, true);
    //setFeature(ads::CDockWidget::CustomCloseHandling, true);
    setFeature(ads::CDockWidget::DockWidgetClosable, false);
    setFeature(ads::CDockWidget::DockWidgetFocusable, true);

    layout()->setContentsMargins(10, 10, 10, 10);

}

void DockWidget::onWindowTitleChanged(const QString &title) {
    if (auto area = dockAreaWidget(); area) {
        area->titleBar()->setWindowTitle(title);
    }
}
