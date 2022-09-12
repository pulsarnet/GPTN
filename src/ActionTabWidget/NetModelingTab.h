//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_NETMODELINGTAB_H
#define FFI_RUST_NETMODELINGTAB_H

#include <QWidget>
#include "../view/graphics_view.h"
#include "../ffi/rust.h"

class NetModelingTab : public QWidget {

public:

    explicit NetModelingTab(QWidget *parent = nullptr);

    ffi::PetriNetContext* ctx() const;
    GraphicsView* view() const;

private:

    GraphicsView *m_view;
    ffi::PetriNetContext* m_ctx;

};


#endif //FFI_RUST_NETMODELINGTAB_H
