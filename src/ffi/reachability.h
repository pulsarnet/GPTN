//
// Created by darkp on 26.09.2022.
//

#ifndef FFI_RUST_REACHABILITY_H
#define FFI_RUST_REACHABILITY_H

#include <vector>
#include "types.h"
#include "vec.h"

namespace rust {
    struct Marking {
        [[nodiscard]] ffi::CVec<int32_t> values() const;
        [[nodiscard]] int32_t prev() const;
        [[nodiscard]] int32_t transition() const;
    };

    struct Reachability {
        [[nodiscard]] ffi::CVec<Marking*> marking() const;
        ~Reachability();
    };
}


#endif //FFI_RUST_REACHABILITY_H
