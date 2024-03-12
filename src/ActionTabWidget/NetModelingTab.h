//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_NETMODELINGTAB_H
#define FFI_RUST_NETMODELINGTAB_H

#include <QWidget>
#include <DockManager.h>

#include "BaseTab.h"

class MainWindow;
class GraphicsView;

class NetModelingTab : public BaseTab {

    Q_OBJECT

public:

    explicit NetModelingTab(MainWindow* window, QWidget *parent = nullptr);

    [[nodiscard]] GraphicsView* view() const;

private:

    MainWindow* m_mainWindow;

    ads::CDockManager* m_dockManager;
    ads::CDockWidget* m_view;

};


#endif //FFI_RUST_NETMODELINGTAB_H
