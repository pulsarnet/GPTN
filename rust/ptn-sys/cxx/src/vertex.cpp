#include <ptn/vertex.h>

using namespace ptn::net::vertex;

extern "C" {
    VertexIndex ptn$net$vertex$index(const Vertex*);
    usize ptn$net$vertex$markers(const Vertex*);
    void ptn$net$vertex$add_marker(Vertex*);
    void ptn$net$vertex$remove_marker(Vertex*);
    void ptn$net$vertex$set_markers(Vertex*, usize);
    char* ptn$net$vertex$label(const Vertex*, bool);
    void ptn$net$vertex$set_label(Vertex*, char*);
    VertexType ptn$net$vertex$type(const Vertex*);
    usize ptn$net$vertex$parent(const Vertex*);
    void ptn$net$vertex$set_parent(Vertex*, usize);
}

VertexIndex Vertex::index() const {
    return ptn$net$vertex$index(this);
}

usize Vertex::markers() const {
    return ptn$net$vertex$markers(this);
}

void Vertex::add_marker() {
    ptn$net$vertex$add_marker(this);
}

void Vertex::remove_marker() {
    ptn$net$vertex$remove_marker(this);
}

void Vertex::set_markers(const usize val) {
    ptn$net$vertex$set_markers(this, val);
}

char* Vertex::label(const bool parent) const {
    return ptn$net$vertex$label(this, parent);
}

void Vertex::set_label(char* val) {
    ptn$net$vertex$set_label(this, val);
}

VertexType Vertex::type() const {
    return ptn$net$vertex$type(this);
}

VertexIndex Vertex::parent() const {
    return VertexIndex { this->type() , ptn$net$vertex$parent(this) };
}

void Vertex::set_parent(const usize val) {
    ptn$net$vertex$set_parent(this, val);
}
