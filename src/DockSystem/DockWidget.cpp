//
// Created by nmuravev on 3/21/2022.
//

#include <QBoxLayout>
#include "DockWidget.h"
#include "DockToolbar.h"


DockWidget::DockWidget(const QString& name, QWidget *parent) : ads::CDockWidget(name, parent) {

    setFrameShape(QFrame::NoFrame);

    setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);

    //qobject_cast<QBoxLayout*>(layout())->insertWidget(0, new DockToolbar(this));

    layout()->setContentsMargins(10, 10, 10, 10);

}
