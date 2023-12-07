#ifndef PTN_EDGE_H
#define PTN_EDGE_H

#include "net.h"

namespace ptn::net::edge {

    struct Connection; //todo rename to Edge

    vertex::VertexIndex from(const Connection* edge);

    vertex::VertexIndex to(const Connection* edge);

    vertex::usize weight(const Connection* edge);

    void set_weight(Connection* edge, vertex::usize weight);

}

#endif
