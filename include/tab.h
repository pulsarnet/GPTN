//
// Created by nmuravev on 12/16/2021.
//

#ifndef FFI_RUST_TAB_H
#define FFI_RUST_TAB_H

#include <QWidget>
#include "graphics_view.h"

class Tab : public QWidget{

    Q_OBJECT

public:

    explicit Tab(QWidget* parent = nullptr);

    GraphicsView* scene();

protected:

private:

    GraphicsView* view = nullptr;

};


#endif //FFI_RUST_TAB_H
