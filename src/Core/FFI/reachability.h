//
// Created by darkp on 26.09.2022.
//

#ifndef FFI_RUST_REACHABILITY_H
#define FFI_RUST_REACHABILITY_H

#include <vector>
#include "types.h"
#include "vec.h"
#include "rust.h"

namespace rust {
    struct Marking {
        [[nodiscard]] ffi::CVec<int32_t> values() const;
        [[nodiscard]] int32_t prev() const;
        [[nodiscard]] ffi::VertexIndex transition() const;
    };

    struct ReachabilityTree {
        [[nodiscard]] ffi::CVec<ffi::VertexIndex> indexes() const;
        [[nodiscard]] ffi::CVec<Marking*> marking() const;
        ~ReachabilityTree();
    };
}


#endif //FFI_RUST_REACHABILITY_H
