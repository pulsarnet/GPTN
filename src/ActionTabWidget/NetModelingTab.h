//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_NETMODELINGTAB_H
#define FFI_RUST_NETMODELINGTAB_H

#include <QWidget>
#include "../Editor/GraphicsView.h"

class MainWindow;

class NetModelingTab : public QWidget {

    Q_OBJECT

public:

    explicit NetModelingTab(MainWindow* window, QWidget *parent = nullptr);

    [[nodiscard]] GraphicsView* view() const;

private:

    GraphicsView *m_view;
    MainWindow* m_mainWindow;

};


#endif //FFI_RUST_NETMODELINGTAB_H
