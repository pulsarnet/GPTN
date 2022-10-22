//
// Created by darkp on 22.10.2022.
//

#ifndef FFI_RUST_QWTCYCLESSCALEDRAW_H
#define FFI_RUST_QWTCYCLESSCALEDRAW_H

#include <QwtScaleDraw>

class QwtCyclesScaleDraw : public QwtScaleDraw {

public:

    explicit QwtCyclesScaleDraw();

    void drawTick(QPainter *, double value, double len) const override;
    void drawLabel(QPainter *, double value) const override;
};


#endif //FFI_RUST_QWTCYCLESSCALEDRAW_H
