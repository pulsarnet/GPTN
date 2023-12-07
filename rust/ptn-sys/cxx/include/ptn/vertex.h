#ifndef PTN_NET_VERTEX
#define PTN_NET_VERTEX

namespace ptn::net::vertex {

    typedef unsigned long long usize;

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