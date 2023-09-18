//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_ACTIONTABWIDGET_H
#define FFI_RUST_ACTIONTABWIDGET_H

#include <QTabWidget>
#include <QToolButton>

class NetModelingTab;
class DecomposeModelTab;

class ActionTabWidget : public QTabWidget {
public:
    explicit ActionTabWidget(QWidget *parent = nullptr);
};


#endif //FFI_RUST_ACTIONTABWIDGET_H
