#ifndef PTN_EDGE_H
#define PTN_EDGE_H

#include <ptn/vertex.h>

namespace ptn::net::edge {

    struct Connection {
        [[nodiscard]] vertex::VertexIndex from() const;

        [[nodiscard]] vertex::VertexIndex to() const;

        [[nodiscard]] usize weight() const;

        void set_weight(usize weight);
    };

}

#endif
