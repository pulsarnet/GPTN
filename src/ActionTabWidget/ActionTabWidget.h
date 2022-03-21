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

public slots:

    void slotTabBarClicked(int index);

    void slotDecompose();
    void slotDDR();
    void slotSLAE();

private:

    QMenu *m_newTabMenu;

    QAction* m_decompose;
    QAction* m_ddr;
    QAction* m_SLAE;

    NetModelingTab* m_netModelingTab;
    DecomposeModelTab* m_decomposeModelTab;
};


#endif //FFI_RUST_ACTIONTABWIDGET_H