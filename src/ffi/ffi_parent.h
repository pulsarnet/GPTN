//
// Created by nmuravev on 1/20/2022.
//
#pragma once
#ifndef FFI_RUST_FFI_PARENT_H
#define FFI_RUST_FFI_PARENT_H

#include <QList>

struct FFIParent;

extern "C" struct FFIParentVec {
    FFIParent *inner;
    unsigned long long len;
};

extern "C" enum FFIVertexType {
    Position,
    Transition
};

extern "C" struct FFIParent {
    FFIVertexType type;
    unsigned long long child;
    unsigned long long parent;

    static QList<FFIParent*> from_ffi_parent_vec(FFIParentVec*);
};

#endif //FFI_RUST_FFI_PARENT_H
