#ifndef PTN_REACHABILITY_H

#define PTN_REACHABILITY_H

#include "opaque.h"
#include "types.h"
#include "vec.h"
#include "vertex.h"

namespace ptn::modules::reachability {
    enum CovType {
        DeadEnd = 0,
        Inner = 1,
        Boundary = 2,
        Duplicate = 3,
    };

    struct Marking: public Opaque {
        [[nodiscard]] alloc::vec::RustVec<i32> values() const;
        [[nodiscard]] CovType type() const;
        [[nodiscard]] i32 prev() const;
        [[nodiscard]] net::vertex::VertexIndex transition() const;

        ~Marking() = delete;
    };

    struct ReachabilityTree: public Opaque {
        alloc::vec::RustVec<net::vertex::VertexIndex> positions() const;
        alloc::vec::RustVec<const Marking*> marking() const;
        void drop();

        ~ReachabilityTree() = delete;
    };
}

#endif