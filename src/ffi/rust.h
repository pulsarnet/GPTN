#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

struct FFIParentVec;

extern "C" struct FFIConnection {
    char* from;
    char* to;
};

extern "C" struct FFIBoxedSlice {
    char** elements;
    unsigned long len_elements;
    FFIConnection** connections;
    unsigned long len_connections;
    unsigned long* markers;
};

extern "C" struct FFIMatrix {
    unsigned long rows_len;
    unsigned long cols_len;
    long* matrix;
};

extern "C" struct FFINamedMatrix {
    char** rows;
    unsigned long rows_len;

    char** cols;
    unsigned long cols_len;
    long* matrix;
};

extern "C" struct FFILogicalBaseFragments {
    FFINamedMatrix* inputs;
    FFINamedMatrix* outputs;
    unsigned long long len;
};

extern "C" struct CommonResult {
    FFIBoxedSlice* petri_net;
    FFIMatrix* c_matrix;
    FFINamedMatrix* lbf_matrix;
    FFILogicalBaseFragments* logical_base_fragments;
    FFIParentVec* parents;
};


#endif //FFI_RUST_RUST_H
