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


extern "C" enum FFIVertexType {
    Position,
    Transition
};

extern "C" struct FFIParent {
    FFIVertexType type;
    unsigned long long child;
    unsigned long long parent;
};

extern "C" struct FFIParentVec {
    FFIParent* inner;
    unsigned long long len;


    [[nodiscard]] QList<FFIParent> into() const {
        QList<FFIParent> result;

        auto cursor = inner;
        for (int i = 0; i < len; i++) {
            result.push_back(*cursor);
            cursor++;
        }

        return result;
    }
};

struct InnerCommonResult {
    InnerPetriNet net;
    Matrix c_matrix;
    NamedMatrix lbf_matrix;
    NamedMatrix d_input;
    NamedMatrix d_output;
    QList<FFIParent> parents;
};

#endif