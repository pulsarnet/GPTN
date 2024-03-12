//
// Created by nmuravev on 3/21/2022.
//

#ifndef FFI_RUST_DOCKWIDGET_H
#define FFI_RUST_DOCKWIDGET_H

#include <DockWidget.h>

class DockWidget : public ads::CDockWidget {

public:

    explicit DockWidget(const QString& name, QWidget *parent = nullptr);

public slots:

    void onFullscreen();

private:

    QAction* m_fullscreenAction;
    bool m_fullscreen = false;
};


#endif //FFI_RUST_DOCKWIDGET_H
