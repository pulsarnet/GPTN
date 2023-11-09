#ifndef FFI_RUST_REACHABILITY_H
#define FFI_RUST_REACHABILITY_H

#include "rust.h"

namespace rust {
    enum MarkingType {
        DeadEnd = 0,
        Inner = 1,
        Boundary = 2,
        Duplicate = 3,
    };

    struct Marking {
        [[nodiscard]] ffi::CVec<int32_t> values() const;
        [[nodiscard]] int32_t prev() const;
        [[nodiscard]] ffi::VertexIndex transition() const;
        [[nodiscard]] MarkingType type() const;
    };

    struct ReachabilityTree {
        [[nodiscard]] ffi::CVec<ffi::VertexIndex> indexes() const;
        [[nodiscard]] ffi::CVec<Marking*> marking() const;
    };

    void reachability_tree_free(ReachabilityTree*);
}


#endif //FFI_RUST_REACHABILITY_H
