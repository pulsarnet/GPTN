#ifndef PTN_NET_VERTEX
#define PTN_NET_VERTEX

#include <ptn/types.h>

namespace ptn::net::vertex {

    enum VertexType {
        Position,
        Transition
    };

    struct VertexIndex {
        VertexType t;
        usize id;
    };


    struct Vertex;

    VertexIndex index(const Vertex *v);
    usize markers(const Vertex *v);

}

#endif