#pragma once
#ifndef FFI_RUST_PETRI_NET_H
#define FFI_RUST_PETRI_NET_H

#include <QString>
#include "matrix.h"
#include "named_matrix.h"

struct FFIParent;
struct FFIBoxedSlice;
struct FFILogicalBaseFragments;
struct CommonResult;

struct InnerPetriNet {
    QList<QString> elements;
    QList<std::pair<QString, QString>> connections;
    QList<unsigned long> markers;

    InnerPetriNet() = default;
    explicit InnerPetriNet(FFIBoxedSlice* boxedSlice);
    explicit InnerPetriNet(FFINamedMatrix* input, FFINamedMatrix* output);

    static QList<InnerPetriNet*> from_ffi_linear_base_fragments(FFILogicalBaseFragments*);
};


struct InnerCommonResult {
    InnerPetriNet net;
    Matrix c_matrix;
    NamedMatrix lbf_matrix;
    QList<InnerPetriNet*> fragments;
    QList<FFIParent*> parents;

    explicit InnerCommonResult(CommonResult*);
};

#endif