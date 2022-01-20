//
// Created by nmuravev on 1/20/2022.
//

#include "ffi_parent.h"

QList<FFIParent*> FFIParent::from_ffi_parent_vec(FFIParentVec *vec) {
    QList<FFIParent*> result;

    auto cursor = vec->inner;
    for (int i = 0; i < vec->len; i++) {
        result.push_back(cursor);
        cursor++;
    }

    return result;
}

