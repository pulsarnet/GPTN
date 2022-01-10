#ifndef FFI_RUST_PETRI_NET_H
#define FFI_RUST_PETRI_NET_H

#include <QString>
#include "matrix.h"
#include "named_matrix.h"

struct InnerPetriNet {
    QList<QString> elements;
    QList<std::tuple<QString, QString>> connections;
    QList<unsigned long> markers;
};

struct InnerCommonResult {
    InnerPetriNet net;
    Matrix c_matrix;
    NamedMatrix d_input;
    NamedMatrix d_output;
    NamedMatrix lbf_matrix;
};

#endif