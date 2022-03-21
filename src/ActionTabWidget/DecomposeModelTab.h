//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <QWidget>
#include "NetModelingTab.h"

class DecomposeModelTab : public QWidget {

public:

    explicit DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent = nullptr);

private:

    NetModelingTab* m_netModelingTab;
    ffi::DecomposeContext* m_ctx;

};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
