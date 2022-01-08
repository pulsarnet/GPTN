#ifndef FFI_RUST_PETRI_NET_H
#define FFI_RUST_PETRI_NET_H

#include <QString>
#include "matrix.h"

struct InnerPetriNet {
    QList<QString> elements;
    QList<std::tuple<QString, QString>> connections;
};

struct InnerCommonResult {
    InnerPetriNet net;
    Matrix matrix;
};

#endif