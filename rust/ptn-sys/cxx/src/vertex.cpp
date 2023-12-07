#include <ptn/vertex.h>

using namespace ptn::net::vertex;

extern "C" {
    VertexIndex ptn$net$vertex$index(const Vertex&);
    usize ptn$net$vertex$markers(const Vertex&);
    void ptn$net$vertex$add_marker(Vertex&);
    void ptn$net$vertex$remove_marker(Vertex&);
    void ptn$net$vertex$set_markers(Vertex&, usize);
    char* ptn$net$vertex$label(const Vertex&, bool);
    void ptn$net$vertex$set_label(Vertex&, char*);
    VertexType ptn$net$vertex$type(const Vertex&);
    usize ptn$net$vertex$parent(const Vertex&);
    void ptn$net$vertex$set_parent(Vertex&, VertexIndex);
}

VertexIndex index(const Vertex* v) {
    return ptn$net$vertex$index(*v);
}

usize markers(const Vertex* v) {
    return ptn$net$vertex$markers(*v);
}
