//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_WRAPPEDLAYOUTWIDGET_H
#define FFI_RUST_WRAPPEDLAYOUTWIDGET_H

#include <QWidget>

class WrappedLayoutWidget : public QWidget {

public:

    explicit WrappedLayoutWidget(QWidget *inner, QWidget* parent = nullptr);


private:

    QWidget* m_inner;
};


#endif //FFI_RUST_WRAPPEDLAYOUTWIDGET_H
