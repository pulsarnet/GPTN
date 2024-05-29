#ifndef PTN_EDGE_H
#define PTN_EDGE_H

#include <ptn/vertex.h>

namespace ptn::net::edge {
    struct DirectedEdge {
        [[nodiscard]] vertex::VertexIndex from() const;
        [[nodiscard]] vertex::VertexIndex to() const;

        void set_weight(usize weight);
        [[nodiscard]] usize weight() const;
    };

    struct InhibitorEdge {
        [[nodiscard]] vertex::VertexIndex place() const;
        [[nodiscard]] vertex::VertexIndex transition() const;
    };
}

#endif
